
package debug is

  procedure BREAKPOINT;

  attribute foreign of BREAKPOINT : procedure is "vhdl_breakpoint";

end package debug;

package body debug is

  procedure BREAKPOINT is
  begin
  end;

end package body debug;
