"""Partition whole lifecycle modules and prove complete shard outcomes."""
import argparse
import hashlib
from collections import Counter
import json
import os
import subprocess
from pathlib import Path


def digest(data):
  return hashlib.sha256(data).hexdigest()


def file_record(path):
  if path.is_symlink():
    return {'sha256': digest(os.fsencode(os.readlink(path))), 'mode': 0o120000}
  if not path.is_file():
    return None
  return {'sha256': digest(path.read_bytes()), 'mode': path.stat().st_mode & 0o777}


def build_identity(root):
  """Pin the complete committed tree plus actual changes; separate only named build outputs."""
  root = Path(root)
  def git(*args):
    return subprocess.check_output(['git', '--no-optional-locks', '-c', 'gc.auto=0', '-c', 'maintenance.auto=false', *args], cwd=root)
  inventory_path = root / 'tools/ci/lifecycle_build_outputs.json'
  inventory_bytes = inventory_path.read_bytes()
  inventory = json.loads(inventory_bytes)
  outputs = {}
  for name, declaration in inventory.items():
    path = Path(name)
    if path.is_absolute() or '..' in path.parts or set(declaration) != {'recipe', 'required_tracked'}:
      raise ValueError('Invalid declared build output')
    outputs[name] = file_record(root / name)
    if declaration['required_tracked'] and outputs[name] is None:
      raise ValueError(f'Missing declared tracked artifact: {name}')
    if not (root / declaration['recipe']).is_file():
      raise ValueError(f'Missing build recipe for {name}')
  changed = git('diff', '--name-only', '--no-renames', '-z', 'HEAD', '--').decode().split('\0')
  changes = {name: file_record(root / name) for name in changed if name and name not in inventory}
  untracked = git('ls-files', '--others', '--exclude-standard', '-z', '--', 'openpilot', 'opendbc_repo', 'panda',
                  'msgq_repo', 'rednose_repo', 'tinygrad_repo', 'tinygrad_model_runtime', 'tools', 'scripts').decode().split('\0')
  additions = {name: file_record(root / name) for name in untracked if name and name not in inventory}
  recipes = {name: file_record(root / name) for name in sorted({'SConstruct', *[row['recipe'] for row in inventory.values()]})}
  tracked = git('ls-files', '--cached', '-z').decode().split('\0')
  source_files = {name: file_record(root / name) for name in tracked if name and name not in inventory}
  if any(metadata is None for metadata in source_files.values()):
    raise ValueError('Missing source input from the tracked build closure')
  inputs = {'revision': git('rev-parse', 'HEAD').decode().strip(), 'tree': git('rev-parse', 'HEAD^{tree}').decode().strip(),
            'tracked_inputs': source_files, 'changes': changes, 'untracked': additions, 'recipes': recipes,
            'output_inventory_sha256': digest(inventory_bytes)}
  return {'inputs': inputs, 'declared_outputs': outputs}


def same_build_inputs(first, second):
  inventory = json.loads((Path(__file__).with_name('lifecycle_build_outputs.json')).read_text())
  expected = digest(Path(__file__).with_name('lifecycle_build_outputs.json').read_bytes())
  for record in (first, second):
    if set(record) != {'inputs', 'declared_outputs'} or set(record['declared_outputs']) != set(inventory):
      raise ValueError('Missing or forged build provenance')
    if set(record['inputs']) != {'revision', 'tree', 'tracked_inputs', 'changes', 'untracked', 'recipes', 'output_inventory_sha256'}:
      raise ValueError('Missing source input closure')
    if not record['inputs']['tracked_inputs'] or not record['inputs']['recipes']:
      raise ValueError('Empty source input closure')
    if record['inputs']['output_inventory_sha256'] != expected:
      raise ValueError('Build output inventory changed')
    for name, metadata in record['declared_outputs'].items():
      if metadata is None and inventory[name]['required_tracked']:
        raise ValueError('Missing native artifact provenance')
      if metadata is not None and (set(metadata) != {'sha256', 'mode'} or len(metadata['sha256']) != 64):
        raise ValueError('Malformed native artifact provenance')
  return first['inputs'] == second['inputs']


def partition(targets, identities, count):
  if count < 1 or len(set(targets)) != len(targets) or len(set(identities)) != len(identities):
    raise ValueError('Invalid lifecycle catalog')
  owners = {}
  for identity in identities:
    matches = [target for target in targets if identity.startswith(target.removesuffix('.py').replace('/', '.') + '.')]
    if len(matches) != 1:
      raise ValueError(f'Lifecycle identity needs exactly one module: {identity}')
    owners[identity] = matches[0]
  weights = Counter(owners.values())
  if set(weights) != set(targets):
    raise ValueError('Lifecycle module collected no cases')
  shards = [[] for _ in range(count)]
  loads = [0] * count
  for target in sorted(targets, key=lambda target: (-weights[target], target)):
    index = min(range(count), key=lambda index: (loads[index], index))
    shards[index].append(target)
    loads[index] += weights[target]
  return shards


def aggregate(directory, count):
  documents = [json.loads(path.read_text()) for path in sorted(Path(directory).rglob('coverage.json'))]
  if len(documents) != count:
    raise ValueError('Missing or duplicate lifecycle shard artifacts')
  first = documents[0]
  full = first['full_catalog_test_ids']
  targets = first['full_catalog_targets']
  expected = partition(targets, full, count)
  seen, indices = [], set()
  for doc in documents:
    index = doc['shard']['index']
    if index in indices or not 0 <= index < count or doc['shard']['count'] != count:
      raise ValueError('Invalid lifecycle shard index')
    indices.add(index)
    if doc['suite'] != 'aol-lifecycle' or doc['status'] != 'completed' or doc['exit_code'] != 0:
      raise ValueError('Lifecycle shard failed')
    if doc['collection_errors'] or doc['gate_errors']:
      raise ValueError('Lifecycle shard errors')
    if doc['full_catalog_test_ids'] != full or doc['full_catalog_targets'] != targets or doc['targets'] != expected[index]:
      raise ValueError('Lifecycle catalog or partition changed')
    for key in ('revision', 'manifest_sha256', 'dependencies', 'runner_sha256', 'suite_runner_sha256', 'pytest_runner_sha256', 'shard_runner_sha256'):
      if doc['source'][key] != first['source'][key]:
        raise ValueError(f'Lifecycle source disagreement: {key}')
    if not same_build_inputs(doc['source']['build_identity'], first['source']['build_identity']):
      raise ValueError('Lifecycle source or native build inputs differ across shards')
    if doc['source']['build_identity'] != doc['source_after']['build_identity']:
      raise ValueError('Lifecycle source or declared native outputs changed during execution')
    if doc['source']['inputs'] != doc['source_after']['inputs']:
      raise ValueError('Lifecycle source changed during execution')
    can_paths = {'opendbc_repo/opendbc/can/packer.py', 'opendbc_repo/opendbc/can/parser.py'}
    can_sources = doc['native'].get('can_sources', {})
    if set(can_sources) != can_paths or can_sources != first['native'].get('can_sources'):
      raise ValueError('Tracked CAN import origins changed')
    for name, checksum in can_sources.items():
      if doc['source']['build_identity']['inputs']['tracked_inputs'][name]['sha256'] != checksum:
        raise ValueError('Imported CAN implementation differs from source inputs')
    capnp = doc['native'].get('pycapnp', {})
    if capnp.get('sha256') != first['native'].get('pycapnp', {}).get('sha256') or len(capnp.get('sha256', '')) != 64:
      raise ValueError('Native pycapnp import provenance changed')
    for key in ('safety', 'aol_protocol'):
      if not doc['native'][key]['sha256']:
        raise ValueError('Missing genuine native build evidence')
    ids = doc['selected_test_ids']
    coverage_path = next(path for path in Path(directory).rglob('coverage.json')
                         if json.loads(path.read_text())['shard']['index'] == index)
    build_input_digest = digest(json.dumps(doc['source']['build_identity']['inputs'], sort_keys=True).encode())
    for kind in ('safety', 'aol_protocol'):
      native = doc['native'][kind]
      if native['build_inputs_sha256'] != build_input_digest:
        raise ValueError('Native build input source provenance changed')
      artifact = coverage_path.with_name(Path(native['path']).name)
      if not artifact.is_file() or digest(artifact.read_bytes()) != native['sha256']:
        raise ValueError('Missing or modified genuine native artifact')
      command = native.get('commands') if kind == 'safety' else native.get('command')
      if not command:
        raise ValueError('Missing actual native builder command')
    result_path = coverage_path.with_name('results.json')
    if doc['result_sha256'] != hashlib.sha256(result_path.read_bytes()).hexdigest():
      raise ValueError('Lifecycle outcome artifact changed')
    result = json.loads(result_path.read_text())
    records = result['results']
    if result['exit_code'] != 0 or result['collection_errors'] or result['gate_errors']:
      raise ValueError('Lifecycle outcome report failed')
    if result['selected_test_ids'] != ids or Counter(row['id'] for row in records) != Counter(ids) or any(row['status'] != 'passed' for row in records):
      raise ValueError('Lifecycle missing, duplicate or unsuccessful outcomes')
    prefixes = tuple(target.removesuffix('.py').replace('/', '.') + '.' for target in expected[index])
    if ids != [identity for identity in full if identity.startswith(prefixes)]:
      raise ValueError('Lifecycle shard cases differ from the full catalog')
    seen.extend(ids)
  if Counter(seen) != Counter(full) or len(set(full)) != len(full):
    raise ValueError('Lifecycle aggregate does not cover every case exactly once')
  return {'status': 'completed', 'shards': count, 'cases_passed': len(full), 'targets': len(targets), 'source_revision': first['source']['revision']}


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument('directory', type=Path)
  parser.add_argument('--count', type=int, default=4)
  parser.add_argument('--output', type=Path, required=True)
  args = parser.parse_args()
  result = aggregate(args.directory, args.count)
  args.output.parent.mkdir(parents=True, exist_ok=True)
  args.output.write_text(json.dumps(result, indent=2) + '\n')
  print(json.dumps(result))


if __name__ == '__main__':
  main()
