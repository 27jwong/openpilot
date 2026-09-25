import { reactive } from "../vendor/vue/vue.esm-browser.js"
import { SETTINGS_SECTIONS } from "./settings.js"

const SPECIAL_PAGES = new Set(["ui_layout", "favorites"])
const PAGE_SECTIONS = new Map(SETTINGS_SECTIONS.flatMap((section) =>
  section.pages.filter((page) => !SPECIAL_PAGES.has(page)).map((page) => [page, section.label])))
const INITIAL_PAGES = [...PAGE_SECTIONS.keys()]

const searchable = (row, title, section) => [row.label, row.value, row.reason, row.unit, title, section]
  .filter((value) => typeof value === "string").join(" ").toLocaleLowerCase()

export function searchSettings(entries, query, limit = 30) {
  const words = query.trim().toLocaleLowerCase().split(/\s+/).filter(Boolean)
  if (!words.length) return []
  return entries.filter((entry) => words.every((word) => entry.search.includes(word)))
    .sort((a, b) => Number(b.label.toLocaleLowerCase().startsWith(words[0])) - Number(a.label.toLocaleLowerCase().startsWith(words[0])) ||
      a.section.localeCompare(b.section) || a.label.localeCompare(b.label)).slice(0, limit)
}

export class SettingsSearchIndex {
  constructor({ fetcher = (...args) => fetch(...args), publish = () => {}, unauthorized = () => {} } = {}) {
    Object.assign(this, { fetcher, publish, unauthorized })
    this.entries = []
    this.status = "idle"
    this.generation = 0
    this.controller = null
    this.pending = null
  }

  stop() {
    this.generation++
    this.controller?.abort()
    this.controller = this.pending = null
    this.entries = []
    this.status = "idle"
    this.publish({ status: "idle", entries: [], failed: 0 })
  }

  async load() {
    if (this.pending) return this.pending
    if (this.status === "ready") return
    const generation = ++this.generation
    const controller = new AbortController()
    this.controller = controller
    this.status = "loading"
    this.entries = []
    this.publish({ status: "loading", entries: [], failed: 0 })
    this.pending = this.collect(generation, controller).finally(() => {
      if (this.generation === generation) this.pending = null
    })
    return this.pending
  }

  async collect(generation, controller) {
    const seen = new Set(INITIAL_PAGES)
    let queue = [...INITIAL_PAGES]
    let failed = 0
    while (queue.length && this.generation === generation) {
      const batch = queue
      queue = []
      let cursor = 0
      const worker = async () => {
        while (cursor < batch.length && this.generation === generation) {
          const page = batch[cursor++]
          let timer
          const timed = new AbortController()
          const abort = () => timed.abort()
          controller.signal.addEventListener("abort", abort, { once: true })
          try {
            timer = setTimeout(abort, 5000)
            const response = await this.fetcher(`./api/settings/pages/${encodeURIComponent(page)}`,
              { credentials: "same-origin", cache: "no-store", signal: timed.signal })
            if (this.generation !== generation) return
            if (response.status === 401) {
              this.stop()
              this.unauthorized()
              return
            }
            if (!response.ok) throw new Error("Settings unavailable")
            const data = await response.json()
            if (this.generation !== generation) return
            if (data?.page !== page || !Array.isArray(data.rows) || typeof data.title !== "string") throw new Error("Invalid settings page")
            const section = PAGE_SECTIONS.get(page.split("/")[0]) || "Toggles"
            for (const row of data.rows) {
              if (typeof row?.label !== "string" || !row.label) continue
              if (typeof row.page === "string" && row.page && !seen.has(row.page) && seen.size < 64) {
                seen.add(row.page)
                queue.push(row.page)
              }
              if (row.page) continue
              this.entries.push({ page, label: row.label, title: data.title, section,
                search: searchable(row, data.title, section) })
            }
            this.publish({ status: "loading", entries: [...this.entries], failed })
          } catch {
            if (this.generation === generation) failed++
          } finally {
            clearTimeout(timer)
            controller.signal.removeEventListener("abort", abort)
          }
        }
      }
      await Promise.all(Array.from({ length: Math.min(4, batch.length) }, worker))
    }
    if (this.generation !== generation) return
    this.status = failed ? "partial" : "ready"
    this.publish({ status: this.status, entries: [...this.entries], failed })
  }
}

export const ToggleSearch = {
  props: { enabled: { type: Boolean, required: true }, unauthorized: { type: Function, required: true },
    openPage: { type: Function, required: true } },
  setup(props) {
    const state = reactive({ query: "", open: false, active: 0, status: "idle", entries: [], failed: 0 })
    const index = new SettingsSearchIndex({ publish: (update) => Object.assign(state, update), unauthorized: props.unauthorized })
    return { state, index }
  },
  computed: {
    results() { return searchSettings(this.state.entries, this.state.query) },
    showResults() { return this.enabled && this.state.open && !!this.state.query.trim() },
  },
  watch: {
    enabled(value) { if (!value) { this.index.stop(); this.state.query = ""; this.state.open = false } },
  },
  beforeUnmount() { this.index.stop() },
  methods: {
    onInput() { this.state.open = true; this.state.active = 0; if (this.enabled) this.index.load() },
    onFocus() { if (this.state.query.trim()) { this.state.open = true; this.index.load() } },
    onBlur(event) { if (!event.currentTarget.contains(event.relatedTarget)) this.state.open = false },
    onKey(event) {
      if (event.key === "Escape") { this.state.query = ""; this.state.open = false; event.preventDefault(); return }
      if (!this.showResults) return
      if (event.key === "ArrowDown" || event.key === "ArrowUp") {
        event.preventDefault()
        this.state.active = (this.state.active + (event.key === "ArrowDown" ? 1 : -1) + this.results.length) % Math.max(1, this.results.length)
      } else if (event.key === "Enter" && this.results.length) { event.preventDefault(); this.choose(this.results[this.state.active] || this.results[0]) }
    },
    choose(hit) { this.state.query = ""; this.state.open = false; this.openPage(hit) },
  },
  template: `
    <div class="gx-searchwrap" @focusout="onBlur">
      <input v-model="state.query" class="gx-search gx-appbar__search" type="search" placeholder="Search toggles…"
        aria-label="Search toggles" :disabled="!enabled" role="combobox" aria-autocomplete="list"
        :aria-expanded="showResults" aria-controls="gx-toggle-search-results"
        :aria-activedescendant="showResults && results.length ? 'gx-toggle-result-' + state.active : undefined"
        @input="onInput" @focus="onFocus" @keydown="onKey">
      <div v-if="showResults" id="gx-toggle-search-results" class="gx-search-results" role="listbox" aria-label="Toggle search results">
        <p v-if="state.status === 'loading'" class="gx-search-results__status" role="status">Searching saved settings…</p>
        <button v-for="(hit, index) in results" :key="hit.page + ':' + hit.label" type="button" role="option"
          :id="'gx-toggle-result-' + index" class="gx-search-results__item" :class="{active:index === state.active}"
          :aria-selected="index === state.active" @click="choose(hit)">
          <strong>{{ hit.label }}</strong><span>{{ hit.section }} · {{ hit.title }}</span>
        </button>
        <p v-if="state.status === 'partial'" class="gx-search-results__status" role="status">Some settings could not be searched. Try again.</p>
        <p v-if="state.status === 'ready' && !results.length" class="gx-search-results__status">No matching saved settings.</p>
      </div>
    </div>`,
}
