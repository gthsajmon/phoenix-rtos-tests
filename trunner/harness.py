from .tools.color import Color


class UnitTestResult:
    """Class representing results of execution of a single Unity unit test."""

    PASS = 'PASS'
    FAIL = 'FAIL'
    IGNORE = 'IGNORE'

    def __init__(self, group, name, status, path='', line='', msg=''):
        self.group = group
        self.name = name
        self.status = status
        self.path = path
        self.line = line
        self.msg = msg

    def __str__(self):
        if self.status == UnitTestResult.PASS:
            color = Color.OK
        elif self.status == UnitTestResult.FAIL:
            color = Color.FAIL
        elif self.status == UnitTestResult.IGNORE:
            color = Color.SKIP

        status = Color.colorify(self.status, color)

        res = f"TEST({self.group}, {self.name}) {status}"
        if self.status == 'FAIL':
            res += f" at {self.path}:{self.line} {self.msg}"

        return res


class BusyboxTestResult:
    """Class representing results of execution of the Busybox test suite."""

    PASS = 'PASS'
    FAIL = 'FAIL'
    IGNORE = 'SKIPPED'

    def __init__(self, name, status, path='', line='', msg=''):
        self.name = name
        self.status = status
        self.msg = msg

    def __str__(self):
        if self.status == BusyboxTestResult.PASS:
            color = Color.OK
        elif self.status == BusyboxTestResult.FAIL:
            color = Color.FAIL
        elif self.status == BusyboxTestResult.IGNORE:
            color = Color.SKIP

        status = Color.colorify(self.status, color)

        res = f"{status}: {self.name}"

        if self.status == 'FAIL':
            res += '\n\t\tTest case verbose output from busybox test suite: \n'
            res += f'{self.msg}'
        return res


class UnitTestHarness:
    """Class providing harness for parsing output of Unity tests"""

    ASSERT = r"ASSERTION (.*?):(\d+):(FAIL|INFO|IGNORE): (.*?)\r"
    RESULT = r"TEST\((\w+), (\w+)\) (PASS|IGNORE)"
    # Fail need to have its own regex due to greedy matching
    RESULT_FAIL = r"TEST\((\w+), (\w+)\) (FAIL) at (.*?):(\d+)\r"
    FINAL = r"(\d+) Tests (\d+) Failures (\d+) Ignored"

    @staticmethod
    def harness(proc):
        test_results = []
        last_assertion = {}
        while True:
            idx = proc.expect([
                UnitTestHarness.ASSERT,
                UnitTestHarness.RESULT,
                UnitTestHarness.RESULT_FAIL,
                UnitTestHarness.FINAL
            ])
            groups = proc.match.groups()
            if idx == 0:
                assertion = dict(zip(('path', 'line', 'status', 'msg'), groups))
                # We only care for fail messages
                if assertion['status'] == 'FAIL':
                    last_assertion = assertion
            elif idx in (1, 2):
                test = dict(zip(('group', 'name', 'status'), groups[:3]))

                # If fail then match info with last assertion
                if test['status'] == 'FAIL':
                    test.update(dict(zip(('path', 'line'), groups[3:])))
                    if (test['path'] in last_assertion.values()
                       and test['line'] in last_assertion.values()):
                        test['msg'] = last_assertion['msg']

                test_results.append(UnitTestResult(**test))
            elif idx == 3:
                test_stats = list(map(int, groups))
                total, fail, ignore = test_stats

                # Make sure if parsed infos are as expected
                fail_no = len([t for t in test_results if t.status == UnitTestResult.FAIL])
                ignore_no = len([t for t in test_results if t.status == UnitTestResult.IGNORE])

                assert fail_no == fail
                assert ignore_no == ignore
                assert len(test_results) == total

                return test_results


class BusyboxTestHarness:
    """Class providing harness for parsing output of the Busybox test suite"""

    RESULT = r"(PASS|SKIPPED|FAIL): (.+?)\r+\n"
    FINAL = r"\*\*\*\*The Busybox Test Suite completed\*\*\*\*\r+\n"
    MESSAGE = r"(.*?)\r+\n"

    @staticmethod
    def harness(proc, self=None):
        passed_nr = 0
        failed_nr = 0
        skipped_nr = 0
        test_results = []
        last_match_was_msg = False
        test = None
        msg = ""

        while True:
            idx = proc.expect([
                BusyboxTestHarness.RESULT,
                BusyboxTestHarness.FINAL,
                BusyboxTestHarness.MESSAGE
            ], timeout=45)
            groups = proc.match.groups()
            if idx == 0:
                if test is not None:
                    if test['status'] == 'FAIL':
                        test['msg'] = msg
                    test_results.append(BusyboxTestResult(**test))

                test = dict(zip(('status', 'name'), groups[:2]))
                test['name'] = 'busybox ' + test['name']
                if test['status'] == 'FAIL':
                    failed_nr = failed_nr + 1
                if test['status'] == 'PASS':
                    passed_nr = passed_nr + 1
                if test['status'] == 'SKIPPED':
                    skipped_nr = skipped_nr + 1
                last_match_was_msg = False
            elif idx == 2:
                if not last_match_was_msg:
                    msg = ""
                msg_next_line = groups[0]
                msg = msg + "\n\t\t" + msg_next_line
                last_match_was_msg = True
            elif idx == 1:
                if test['status'] == 'FAIL':
                    test['msg'] = msg
                test_results.append(BusyboxTestResult(**test))

                test_stats = [passed_nr, failed_nr, skipped_nr]

                total, fail, ignore = test_stats

                return test_results
