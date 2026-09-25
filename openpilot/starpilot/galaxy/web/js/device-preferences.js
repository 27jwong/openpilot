import { DriveStatePanel } from "./drive-state.js"
export const DEVICE_PAGES = Object.freeze({})
export function devicePage(path) { return null }
export const DevicePreferencesPage = {
  components: { DriveStatePanel },
  props: { mode: { type: String, required: true }, go: { type: Function, required: true } },
  template: `<section class="gx-driving" aria-label="Force Drive State"><DriveStatePanel v-if="mode === 'local'" /><div v-else class="gx-card gx-message">Force Drive State is available on your connected device.</div></section>`,
}
