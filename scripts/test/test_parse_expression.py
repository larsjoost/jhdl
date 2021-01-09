import unittest

from parse_expression import ParseExpression

class TestParseExpression(unittest.TestCase):

    def test_convert_variables_to_cmake_syntax(self):
        p = ParseExpression()
        self.assertEqual(p.convert_variables_to_cmake_syntax("$JHDL/src"), "${JHDL}/src")
        self.assertEqual(p.convert_variables_to_cmake_syntax("$JHDL_TEST/src"), "${JHDL_TEST}/src")
        self.assertEqual(p.convert_variables_to_cmake_syntax("${JHDL}/src"), "${JHDL}/src")
        self.assertEqual(p.convert_variables_to_cmake_syntax("${jhdl}/src"), "${jhdl}/src")
        self.assertEqual(p.convert_variables_to_cmake_syntax("$JHDL1"), "${JHDL1}")
