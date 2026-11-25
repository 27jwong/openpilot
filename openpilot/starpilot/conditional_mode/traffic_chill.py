"""Traffic Mode holds chill.

While the driver has Traffic Mode on, plannerd hands the conditional host a forced-chill manual override
instead of the driver's own manual state. It replaces CEM's and CCM's automatic triggers, the driver's
saved manual experimental toggle and the GEN2 crawl zone's experimental demand, so Traffic Mode's follow
time and jerk drive through the ordinary MPC instead of the model's e2e plan.

Nothing is saved or changed: the override is rebuilt every planner cycle, so when Traffic Mode goes off it
simply disappears and the saved choice (CEM, its manual state, the crawl zone) applies again.

Two things it does not reach, on purpose: a CEM stop already committed before Traffic Mode came on plays
out as before (base CEM puts a committed stop ahead of any manual intent, and the stop detector cannot
commit a new one while Traffic Mode is on), and the plain Experimental Mode toggle without conditional mode
(that path never consults the conditional host).
"""

from openpilot.starpilot.conditional_mode.policy import ManualIntent
from openpilot.starpilot.conditional_mode.preferences import ManualState, manual_for_drive


def traffic_chill(verdict, drive_id: int, now_ns: int) -> ManualState | None:
  """The forced-chill override while Traffic Mode is requested this drive, else None.

  Uses the driver's request, not its authority-gated effect, so a gas override or a disengage-reengage
  inside Traffic Mode cannot bring experimental back for the frames in between."""
  if getattr(verdict, 'requested', None) is not True:
    return None
  return manual_for_drive(ManualIntent.FORCE_CHILL, drive_id, now_ns)
