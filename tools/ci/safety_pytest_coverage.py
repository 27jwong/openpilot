"""Run omitted native pytest cases serially against the gate's instrumented library."""
import argparse
import ast
import hashlib
import json
from pathlib import Path
import sys
from unittest import mock

from tools.ci import vehicle_pytest

ROOT = Path(__file__).resolve().parents[2]
# These denial classes belong to the existing mandatory RELEASE vehicle plan.
RELEASE_ONLY = frozenset((
  'opendbc_repo/opendbc/safety/tests/test_hyundai_main_rearm.py::TestHkgMainRelease',
  'opendbc_repo/opendbc/safety/tests/test_hyundai_classic_cancel_joined.py::TestClassicCancelReleaseDenied',
))


def targets_for_coverage(root=ROOT):
  native_root = root / 'opendbc_repo/opendbc/safety/tests'
  selected = set()
  # unittest already owns its TestCase subclasses, including decorated classes.
  for path in sorted(native_root.glob('test_*.py')):
    relative = path.relative_to(root).as_posix()
    for node in ast.parse(path.read_text(), filename=str(path)).body:
      function = isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)) and node.name.startswith('test_')
      plain_class = isinstance(node, ast.ClassDef) and node.name.startswith('Test') and not node.bases
      if function or plain_class:
        selected.add(f'{relative}::{node.name}')
  # These existing end-to-end callers bind the supplied library. Other vehicle suites
  # deliberately compile distinct DEBUG/RELEASE artifacts and retain their own gates.
  for relative in ('openpilot/starpilot/car/nissan/tests/test_aol.py',
                   'openpilot/starpilot/car/rivian/tests/test_aol_native.py'):
    if (root / relative).is_file():
      selected.add(relative)
  return sorted(selected - RELEASE_ONLY)


def report_complete(report, targets, native_sha256):
  if (not isinstance(report, dict) or type(report.get('schema_version')) is not int or
      report.get('schema_version') != 1 or report.get('full_catalog') is not True):
    return False
  ids, records = report.get('nodeids'), report.get('records')
  if (report.get('targets') != targets or not targets or report.get('native_sha256') != native_sha256 or
      report.get('native_bound_before') is not True or report.get('native_bound_after') is not True or
      report.get('collection_errors') != [] or report.get('execution_errors') != [] or
      not isinstance(ids, list) or not ids or not all(type(nodeid) is str for nodeid in ids) or len(set(ids)) != len(ids) or not isinstance(records, list)):
    return False
  if not all(isinstance(row, dict) and row.get('status') in ('passed', 'skipped') for row in records):
    return False
  actual = [row.get('id') for row in records]
  return (all(type(identity) is str for identity in actual) and len(actual) == len(ids) and len(set(actual)) == len(actual) and
          set(actual) == {vehicle_pytest.test_id(nodeid) for nodeid in ids} and any(row['status'] == 'passed' for row in records))


def run(native, output, targets=None):
  from opendbc.safety.tests.libsafety import libsafety_py
  native = Path(native).resolve()
  selected = targets_for_coverage()
  full_catalog = targets is None
  targets = selected if targets is None else list(targets)
  if not targets or any(not any(target == item or target.startswith(item + '::') for item in selected) for target in targets):
    raise ValueError('Coverage subset must name existing native catalog cases')
  digest = hashlib.sha256(native.read_bytes()).hexdigest()
  libsafety_py.load(native)
  original_load = libsafety_py.load

  def load_pinned(path):
    if Path(path).resolve() != native:
      raise RuntimeError('Native coverage cases must retain the same instrumented libsafety')
    return original_load(native)

  report = {'schema_version': 1, 'full_catalog': full_catalog, 'targets': targets, 'native_sha256': digest,
            'native_path': str(native), 'native_bound_before': Path(libsafety_py.libpath).resolve() == native,
            'nodeids': [], 'records': [], 'collection_errors': [], 'execution_errors': []}
  with mock.patch.object(libsafety_py, 'load', side_effect=load_pinned):
    nodeids, errors = vehicle_pytest.collect(targets)
    report.update(nodeids=nodeids, collection_errors=errors)
    if not errors:
      records, errors = vehicle_pytest.run(nodeids)
      report.update(records=records, execution_errors=errors)
  report['native_bound_after'] = Path(libsafety_py.libpath).resolve() == native and hashlib.sha256(native.read_bytes()).hexdigest() == digest
  Path(output).write_text(json.dumps(report, indent=2) + '\n')
  # The root's bounded native subset is a diagnostic; production always requires the complete catalog.
  checked = dict(report, full_catalog=True)
  return report_complete(checked, targets, digest)


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument('--native', required=True)
  parser.add_argument('--results-json', required=True)
  parser.add_argument('--target', action='append')
  args = parser.parse_args()
  return 0 if run(args.native, args.results_json, args.target) else 1


if __name__ == '__main__':
  sys.exit(main())
