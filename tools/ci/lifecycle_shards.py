"""Partition whole lifecycle modules and prove complete shard outcomes."""
import argparse
import hashlib
from collections import Counter
import json
from pathlib import Path


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
    for key in ('revision', 'inputs', 'manifest_sha256', 'dependencies', 'runner_sha256', 'suite_runner_sha256', 'pytest_runner_sha256', 'shard_runner_sha256'):
      if doc['source'][key] != first['source'][key]:
        raise ValueError(f'Lifecycle source disagreement: {key}')
    if doc['source']['inputs'] != doc['source_after']['inputs']:
      raise ValueError('Lifecycle source changed during execution')
    for key in ('safety', 'aol_protocol'):
      if not doc['native'][key]['sha256']:
        raise ValueError('Missing genuine native build evidence')
    ids = doc['selected_test_ids']
    coverage_path = next(path for path in Path(directory).rglob('coverage.json')
                         if json.loads(path.read_text())['shard']['index'] == index)
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
