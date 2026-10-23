import copy
import hashlib
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('private_lifecycle_shards', Path(__file__).parents[1] / 'lifecycle_shards.py')
assert spec is not None and spec.loader is not None
helper = importlib.util.module_from_spec(spec)
spec.loader.exec_module(helper)


class LifecycleShardTests(unittest.TestCase):
  def catalog(self):
    targets = ['a.py', 'b.py', 'c.py', 'd.py']
    identities = [f'{name}.Test.test_{i}' for name in 'abcd' for i in range(3)]
    return targets, identities

  def test_partition_preserves_whole_modules_and_all_cases(self):
    targets, identities = self.catalog()
    shards = helper.partition(targets, identities, 4)
    self.assertEqual(sorted(target for shard in shards for target in shard), targets)
    self.assertEqual([len(shard) for shard in shards], [1] * 4)
    self.assertEqual(shards, helper.partition(list(reversed(targets)), identities, 4))

  def test_invalid_catalogs_are_rejected(self):
    targets, identities = self.catalog()
    for bad_targets, bad_ids, count in [(targets, identities, 0), (targets + targets, identities, 4),
                                        (targets, identities + identities, 4), (targets, ['foreign.test'], 4),
                                        (targets + ['empty.py'], identities, 4)]:
      with self.subTest(targets=bad_targets, count=count), self.assertRaises(ValueError):
        helper.partition(bad_targets, bad_ids, count)

  def documents(self):
    targets, full = self.catalog()
    partition = helper.partition(targets, full, 4)
    source = dict.fromkeys(('revision', 'inputs', 'manifest_sha256', 'dependencies', 'runner_sha256',
                                    'suite_runner_sha256', 'pytest_runner_sha256', 'shard_runner_sha256'), 'same')
    docs = []
    for index, assigned in enumerate(partition):
      ids = [identity for identity in full if identity.startswith(assigned[0].removesuffix('.py') + '.')]
      doc = {'suite': 'aol-lifecycle', 'status': 'completed', 'exit_code': 0, 'shard': {'index': index, 'count': 4},
             'targets': assigned, 'selected_test_ids': ids, 'full_catalog_targets': targets, 'full_catalog_test_ids': full,
             'collection_errors': [], 'gate_errors': [], 'source': source, 'source_after': source,
             'native': {'safety': {'sha256': '1' * 64}, 'aol_protocol': {'sha256': '2' * 64}}}
      result = {'selected_test_ids': ids, 'exit_code': 0, 'collection_errors': [], 'gate_errors': [],
                'results': [{'id': identity, 'status': 'passed'} for identity in ids]}
      docs.append((doc, result))
    return docs

  def check(self, docs):
    with tempfile.TemporaryDirectory() as directory:
      for index, (doc, result) in enumerate(docs):
        path = Path(directory) / str(index)
        path.mkdir()
        (path / 'results.json').write_text(json.dumps(result))
        doc['result_sha256'] = hashlib.sha256((path / 'results.json').read_bytes()).hexdigest()
        (path / 'coverage.json').write_text(json.dumps(doc))
      return helper.aggregate(directory, 4)

  def test_complete_catalog_passes(self):
    self.assertEqual(self.check(self.documents())['cases_passed'], 12)

  def test_bad_outcomes_and_provenance_are_denied(self):
    mutations = [lambda d: d.pop(),
                 lambda d: d[0][0]['shard'].update(index=1),
                 lambda d: d[0][0].update(exit_code=1),
                 lambda d: d[0][0].update(gate_errors=['failed gate']),
                 lambda d: d[0][0]['full_catalog_test_ids'].pop(),
                 lambda d: d[0][0].update(targets=['foreign.py']),
                 lambda d: d[0][0]['source'].update(revision='foreign'),
                 lambda d: d[0][0]['source_after'].update(inputs='changed'),
                 lambda d: d[0][0]['native']['safety'].update(sha256=''),
                 lambda d: d[0][1]['results'].pop(),
                 lambda d: d[0][1]['results'].append(d[0][1]['results'][0]),
                 lambda d: d[0][1]['results'][0].update(status='skipped'),
                 lambda d: d[0][1]['results'][0].update(status='failed'),
                 lambda d: d[0][1].update(exit_code=1)]
    for index, mutate in enumerate(mutations):
      docs = copy.deepcopy(self.documents())
      # Break intentional shared catalog/source aliases to exercise genuine cross-shard disagreement.
      docs = json.loads(json.dumps(docs))
      mutate(docs)
      with self.subTest(index=index), self.assertRaises((ValueError, KeyError, StopIteration)):
        self.check(docs)


if __name__ == '__main__':
  unittest.main()
