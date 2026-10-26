# SpacePilot

In native Developer settings, tap the **version number** five times. Both the
comma four and Big UI provide the same gesture and native game. Release each tap
within 450 ms without dragging; consecutive taps must be 100–850 ms apart, all
within three seconds. Scrolling, another row, navigation, a long press, or loss
of physical parked evidence resets progress.

Keep the vehicle in Park and disengaged while the preparation screen verifies
its state and stops driving services. Play begins only after those services are
confirmed stopped and every connected Panda reports healthy `noOutput`.
Tap to start, boost, and retry. With ignition on, the wheel's + and − are observed
through an independent receive-only CAN decoder. Release a wheel button once to
arm it; each following press boosts once. Held/repeated inputs do not repeat.

Offroad with ignition off, use touch or a paired USB/Bluetooth controller. Enable
and configure the controller's existing **Increase Set Speed** / **Decrease Set
Speed** bindings in Controller settings. In SpacePilot those bindings only boost
the ship; no cruise request is dispatched. All controller input in the game's
input batch is consumed, even if the first press closes the game. Exiting restores
ordinary controller routing, subject to its original control authority checks.

## Lifecycle and physical evidence

Onroad admission retains the original one-second fresh physical Park/standstill,
zero ego/raw/all wheel speed, valid CAN, disengaged ACC/controls, healthy connected
Panda, and no native alert requirements. Replay/projection, unknown state and
host overrides cannot admit the game. The version row owns only the gesture;
the page owns its own admission and cleanup state.

The game requests Offroad through the existing session-bound `DriveStateOwner`.
It does not falsify vehicle state, change manager process predicates or native
safety, initialize a vehicle interface/controller, send CAN, or alter alerts.
Normal `pandad` retains raw CAN reception offroad and selects `noOutput`. A
separate bounded subscriber instantiates only the current vehicle's normal
`CarState` and CAN parsers, retaining checksum/counter/timeout checks. Required
CAN frames additionally must remain within 250 ms; unrelated packets cannot keep
a remembered Park or zero speed alive. Unsupported or incomplete decoding fails
closed. Current live-session `carParams` is required for ignition-on play.

The game waits for actual `card`, `selfdrived`, `controlsd`, `modeld`, and
`dmonitoringmodeld` shutdown. Fresh manager/device/Panda source and receipt clocks,
physical noOutput, independent Park evidence, clock continuity and input bounds
remain mandatory. With ignition off, fresh healthy noOutput and actual stopped
processes provide the existing physical offroad premise; wheel input is disabled.
A mode request alone never proves that the car is parked. Changing ignition,
mode, gear, speed, engagement, process state, faults, source freshness or clock
continuity ends the game. Entry/shutdown times out after eight seconds.

A successful onroad entry **leaves Offroad selected when the game exits or the UI
closes**. Returning to driving requires an explicit existing drive-mode selection
(Onroad or Auto), which performs the normal offroad/onroad startup. The game never
restores driving services automatically. This is the existing manager-session
request, not a new persistent safety mode across manager/system restarts.

## Validation boundary

Tests cover physical admission, stale source/receipt clocks, process/noOutput
transitions, the actual drive-state owner, touch/wheel/controller isolation,
normal CAN decoding and refusal, both native version gestures, bounded simulation
and cleanup. Native GL previews and host CPU/RSS/frame-time measurements use
isolated synthetic telemetry. They do not establish physical C4/C3 performance,
actual wheel/controller hardware behavior, full-system headroom or deployment.
No road-device installation, restart, or driving test is part of host validation.
