// Local recording inventory and closed Quick road playback only.
const LOCAL = /^(?:[a-f0-9]{8}--[a-f0-9]{10}|[0-9]{4}-[0-9]{2}-[0-9]{2}--[0-9]{2}-[0-9]{2}-[0-9]{2}|[a-f0-9]{16}\|(?:[a-f0-9]{8}--[a-f0-9]{10}|[0-9]{4}-[0-9]{2}-[0-9]{2}--[0-9]{2}-[0-9]{2}-[0-9]{2}))$/
const SEGMENT = /^(?:[a-f0-9]{8}--[a-f0-9]{10}|[0-9]{4}-[0-9]{2}-[0-9]{2}--[0-9]{2}-[0-9]{2}-[0-9]{2}|[a-f0-9]{16}[|_](?:[a-f0-9]{8}--[a-f0-9]{10}|[0-9]{4}-[0-9]{2}-[0-9]{2}--[0-9]{2}-[0-9]{2}-[0-9]{2}))--[0-9]{1,6}$/
const FILES = ["rlog", "qlog", "fcamera", "dcamera", "ecamera", "qcamera"]
const LABELS = { rlog: "Full log", qlog: "Quick log", fcamera: "Road video", dcamera: "Driver video",
  ecamera: "Wide video", qcamera: "Quick video" }

export function validLocalHistory(value) {
  if (value?.schemaVersion !== 1 || value.source !== "local" || value.partialHistory !== true ||
      typeof value.scanIncomplete !== "boolean" || !Array.isArray(value.routes) || value.routes.length > 100) return false
  let count = 0
  const routes = new Set()
  return value.routes.every((route) => {
    if (typeof route?.routeId !== "string" || !LOCAL.test(route.routeId) || routes.has(route.routeId) ||
        !Array.isArray(route.segments) || route.segments.length > 64 ||
        !Number.isSafeInteger(route.segmentCount) || route.segmentCount !== route.segments.length) return false
    routes.add(route.routeId)
    const numbers = new Set()
    count += route.segmentCount
    return count <= 512 && route.segments.every((segment) => {
      if (!Number.isSafeInteger(segment?.number) || segment.number < 0 || segment.number > 999999 ||
          typeof segment.segmentName !== "string" || !SEGMENT.test(segment.segmentName) ||
          (!segment.segmentName.startsWith(route.routeId.replace('|', '_') + '--') &&
           !segment.segmentName.startsWith(route.routeId + '--')) ||
          Number(segment.segmentName.slice(segment.segmentName.lastIndexOf('--') + 2)) !== segment.number ||
          numbers.has(segment.number) || segment.files === null || typeof segment.files !== "object" ||
          Object.keys(segment.files).length !== FILES.length ||
          !FILES.every((key) => typeof segment.files[key] === "boolean") ||
          !FILES.some((key) => segment.files[key])) return false
      numbers.add(segment.number)
      return true
    })
  })
}

export const availableFiles = (files) => FILES.filter((key) => files?.[key]).map((key) => LABELS[key])
export const routeFiles = (route) => availableFiles(Object.fromEntries(FILES.map((key) =>
  [key, route.segments.some((segment) => segment.files[key])])))
export const firstQuickVideo = (route) => route.segments.find((segment) => segment.files.qcamera) || null
export function routeDate(route, locale = undefined, timeZone = undefined) {
  const validTime = (value) => typeof value === "number" && Number.isFinite(value) && value > 0 && Number.isFinite(new Date(value * 1000).getTime())
  const captured = validTime(route.startTime)
  const value = captured ? route.startTime : validTime(route.fileTime) ? route.fileTime : null
  if (value === null) return { label: "Date unavailable", source: "", datetime: null }
  const date = new Date(value * 1000)
  return { label: new Intl.DateTimeFormat(locale, { year: "numeric", month: "short", day: "numeric",
    hour: "numeric", minute: "2-digit", ...(timeZone ? { timeZone } : {}) }).format(date),
    source: captured ? "" : "File date", datetime: date.toISOString() }
}
export function connectRouteUrl(route) {
  if (typeof route?.routeId !== "string" || !LOCAL.test(route.routeId) || typeof route.connectUrl !== "string") return null
  const host = route.provider === "konik" ? "stable.konik.ai" : route.provider === undefined || route.provider === "comma" ? "connect.comma.ai" : null
  if (host === null) return null
  const match = /^https:\/\/(connect\.comma\.ai|stable\.konik\.ai)\/([a-f0-9]{16})\/([^/?#]+)$/.exec(route.connectUrl)
  const [device, identifier] = route.routeId.includes('|') ? route.routeId.split('|') : [null, route.routeId]
  return match && match[1] === host && match[3] === identifier && (device === null || match[2] === device) ? route.connectUrl : null
}
export const quickRoadUrl = (segmentName) => SEGMENT.test(segmentName) ?
  `./api/recordings/media/${encodeURIComponent(segmentName)}` : null
export const segmentSummaryUrl = (segmentName) => SEGMENT.test(segmentName) ?
  `./api/recordings/segment-summary?segmentName=${encodeURIComponent(segmentName)}` : null

export function validSegmentSummary(value, segmentName) {
  const metric = (v) => v === null || (typeof v === "number" && Number.isFinite(v) && v >= 0)
  return value?.schemaVersion === 1 && value.source === "closed_local_rlog" &&
    value.segmentName === segmentName && typeof value.sourceSha256 === "string" &&
    /^[a-f0-9]{64}$/.test(value.sourceSha256) &&
    [value.observedCarSpanSeconds, value.estimatedDistanceMeters,
      value.observedLatActiveSeconds, value.observedLongActiveSeconds].every(metric) &&
    value.gaps && Number.isSafeInteger(value.gaps.carState) && value.gaps.carState >= 0 &&
    Number.isSafeInteger(value.gaps.carControl) && value.gaps.carControl >= 0 &&
    typeof value.sampleCoverageComplete === "boolean"
}

export const detailMetric = (seconds, unit = "s") => seconds === null ? "Unavailable" :
  `${seconds.toFixed(unit === "m" ? 1 : 2)} ${unit}`

export class LocalHistoryFeed {
  static endpoint = "./api/recordings/local"
  static valid = validLocalHistory
  static subject = "local recordings"
  static unavailable = "Local recordings are unavailable."
  static invalid = "Local recording inventory is unavailable."

  constructor({ publish, unauthorized = () => {}, fetcher = (...args) => fetch(...args),
                later = (fn, ms) => setTimeout(fn, ms), cancelTimer = (id) => clearTimeout(id) }) {
    Object.assign(this, { publish, unauthorized, fetcher, later, cancelTimer })
    this.active = false
    this.generation = 0
    this.request = null
    this.timer = null
    this.status = "idle"
    this.data = null
    this.error = ""
  }

  emit() { this.publish({ status: this.status, data: this.data, error: this.error }) }
  stop() {
    this.active = false
    this.generation++
    this.request?.abort()
    if (this.timer !== null) this.cancelTimer(this.timer)
    this.request = this.timer = null
    this.status = "idle"
    this.data = null
    this.error = ""
    this.emit()
  }
  start() { this.stop(); this.active = true; return this.load() }

  async load() {
    if (!this.active || this.request !== null) return
    const generation = this.generation
    const request = new AbortController()
    this.request = request
    this.status = "loading"
    this.error = ""
    this.emit()
    this.timer = this.later(() => {
      if (!this.active || generation !== this.generation || this.request !== request) return
      request.abort()
      this.generation++
      this.request = this.timer = null
      this.data = null
      this.status = "unavailable"
      this.error = `Reading ${this.constructor.subject} timed out. Refresh to try again.`
      this.emit()
    }, 4000)
    try {
      const response = await this.fetcher(this.constructor.endpoint, { credentials: "same-origin", cache: "no-store", signal: request.signal })
      if (!this.active || generation !== this.generation || request.signal.aborted) return
      if (response.status === 401) { this.stop(); this.unauthorized(); return }
      if (response.status === 503) {
        const body = await response.json().catch(() => null)
        if (!this.active || generation !== this.generation || request.signal.aborted) return
        if (["setup_required", "access_unavailable"].includes(body?.code)) { this.stop(); this.unauthorized(); return }
        throw new Error(this.constructor.unavailable)
      }
      if (!response.ok) throw new Error(this.constructor.unavailable)
      const data = await response.json()
      if (!this.active || generation !== this.generation || request.signal.aborted) return
      if (!this.constructor.valid(data)) throw new Error(this.constructor.invalid)
      this.data = data
      this.status = "ready"
      this.emit()
    } catch (error) {
      if (this.active && generation === this.generation) {
        this.data = null
        this.status = "unavailable"
        this.error = error instanceof Error ? error.message : this.constructor.unavailable
        this.emit()
      }
    } finally {
      if (generation === this.generation) {
        if (this.timer !== null) this.cancelTimer(this.timer)
        this.timer = null
        this.request = null
      }
    }
  }
}

export const LocalRecordingsPage = {
  name: "LocalRecordingsPage",
  props: { mode: { type: String, required: true }, unauthorized: { type: Function, required: true } },
  data: () => ({ status: "idle", data: null, error: "", playing: null, playerError: "", playerGeneration: 0,
    details: null, detailsName: null, detailsStatus: "idle", detailsError: "", detailsGeneration: 0 }),
  computed: {
    routeCards() {
      return (this.data?.routes || []).map((route) => ({ ...route, date: routeDate(route),
        connect: connectRouteUrl(route), fileLabels: routeFiles(route), firstQuick: firstQuickVideo(route) }))
    },
  },
  created() { this.feed = new LocalHistoryFeed({ publish: (update) => Object.assign(this.$data, update), unauthorized: this.unauthorized }) },
  mounted() {
    this.visibility = () => { if (document.hidden) { this.closePlayer(); this.closeDetails(); this.feed.stop() } else if (this.mode === "local") this.feed.start() }
    document.addEventListener("visibilitychange", this.visibility)
    if (this.mode === "local" && !document.hidden) this.feed.start()
  },
  beforeUnmount() { document.removeEventListener("visibilitychange", this.visibility); this.closePlayer(); this.closeDetails(); this.feed.stop() },
  watch: { mode(value) { if (value !== "local") { this.closePlayer(); this.closeDetails() } } },
  methods: {
    availableFiles,
    quickRoadUrl,
    detailMetric,
    refresh() { this.closePlayer(); this.closeDetails(); return this.feed.load() },
    closeDetails() {
      this.detailsRequest?.abort()
      if (this.detailsTimer !== undefined) clearTimeout(this.detailsTimer)
      this.detailsRequest = null
      this.detailsTimer = undefined
      this.detailsGeneration++
      this.details = null
      this.detailsName = null
      this.detailsStatus = "idle"
      this.detailsError = ""
    },
    async openDetails(segment) {
      if (this.mode !== "local" || this.status !== "ready" || !segment?.files?.rlog || document.hidden) return
      const url = segmentSummaryUrl(segment.segmentName)
      if (!url) return
      this.closeDetails()
      const generation = this.detailsGeneration
      const controller = new AbortController()
      this.detailsRequest = controller
      this.detailsName = segment.segmentName
      this.detailsStatus = "loading"
      this.detailsTimer = setTimeout(() => {
        if (generation !== this.detailsGeneration || this.detailsRequest !== controller) return
        controller.abort()
        this.detailsRequest = null
        this.detailsGeneration++
        this.detailsTimer = undefined
        this.detailsStatus = "unavailable"
        this.detailsError = "Reading this segment timed out. Select Details to try again."
      }, 10000)
      try {
        const response = await fetch(url, { credentials: "same-origin", cache: "no-store", signal: controller.signal })
        if (generation !== this.detailsGeneration || controller.signal.aborted) return
        if (response.status === 401) { this.closeDetails(); this.unauthorized(); return }
        if (response.status === 503) {
          const body = await response.json().catch(() => null)
          if (generation !== this.detailsGeneration || controller.signal.aborted) return
          if (["setup_required", "access_unavailable"].includes(body?.code)) { this.closeDetails(); this.unauthorized(); return }
        }
        if (!response.ok) throw new Error("Recorded segment details are unavailable.")
        const result = await response.json()
        if (generation !== this.detailsGeneration || controller.signal.aborted) return
        if (!validSegmentSummary(result, segment.segmentName)) throw new Error("Recorded segment details are unavailable.")
        this.details = result
        this.detailsStatus = "ready"
        this.$nextTick?.(() => this.$refs.detailsPanel?.scrollIntoView?.({ block: "nearest", behavior: "smooth" }))
      } catch (error) {
        if (generation === this.detailsGeneration) {
          this.detailsStatus = "unavailable"
          this.detailsError = error instanceof Error ? error.message : "Recorded segment details are unavailable."
        }
      } finally {
        if (generation === this.detailsGeneration) {
          if (this.detailsTimer !== undefined) clearTimeout(this.detailsTimer)
          this.detailsTimer = undefined
          this.detailsRequest = null
        }
      }
    },
    openPlayer(route, segment) {
      if (this.mode !== "local" || this.status !== "ready" || !segment.files.qcamera) return
      const segments = route.segments.filter((item) => item.files.qcamera && quickRoadUrl(item.segmentName))
      const index = segments.findIndex((item) => item.segmentName === segment.segmentName)
      if (index < 0) return
      this.closePlayer()
      this.playing = { routeId: route.routeId, segments, index, url: quickRoadUrl(segment.segmentName) }
      this.playerGeneration++
      this.$nextTick?.(() => this.$refs.playerPanel?.scrollIntoView?.({ block: "nearest", behavior: "smooth" }))
    },
    chooseSegment(offset) {
      if (!this.playing) return
      const index = this.playing.index + offset
      if (index < 0 || index >= this.playing.segments.length) return
      this.sessionProbe?.abort()
      this.sessionProbe = null
      this.$refs.quickVideo?.pause()
      this.$refs.quickVideo?.removeAttribute("src")
      this.playing = { ...this.playing, index, url: quickRoadUrl(this.playing.segments[index].segmentName) }
      this.playerError = ""
      this.playerGeneration++
    },
    closePlayer() {
      this.sessionProbe?.abort()
      this.sessionProbe = null
      this.$refs.quickVideo?.pause()
      this.$refs.quickVideo?.removeAttribute("src")
      this.$refs.quickVideo?.load()
      this.playing = null
      this.playerError = ""
      this.playerGeneration++
    },
    async videoError(event) {
      if (!this.playing || event.currentTarget !== this.$refs.quickVideo ||
          Number(event.currentTarget.dataset.playerGeneration) !== this.playerGeneration) return
      const generation = this.playerGeneration
      this.playerError = "This Quick road recording could not play in this browser."
      const controller = new AbortController()
      this.sessionProbe?.abort()
      this.sessionProbe = controller
      const deadline = setTimeout(() => controller.abort(), 4000)
      let revoke = false
      try {
        const response = await fetch("./api/auth/session", { credentials: "same-origin", cache: "no-store", signal: controller.signal })
        if (!this.playing || generation !== this.playerGeneration || controller.signal.aborted) return
        const state = await response.json()
        if (!this.playing || generation !== this.playerGeneration || controller.signal.aborted) return
        revoke = response.status === 401 || state.authenticated === false
      } catch { /* A media or session request may fail; keep the explicit playback error. */ }
      finally { clearTimeout(deadline); if (this.sessionProbe === controller) this.sessionProbe = null }
      if (revoke && this.playing && generation === this.playerGeneration) { this.closePlayer(); this.unauthorized() }
    },
  },
  template: `
    <div class="gx-view gx-recordings">
      <h2>Local Recordings</h2>
      <p class="gx-note">Drives and video saved on this device. Older recordings may have been removed to free space.</p>
      <p v-if="mode !== 'local'" class="gx-card gx-message">Local recordings are unavailable in the offline preview.</p>
      <template v-else>
        <div class="gx-recordings__toolbar">
          <button type="button" class="gx-btn gx-btn--tonal" :disabled="status === 'loading'" @click="refresh">Refresh</button>
          <a class="gx-btn gx-btn--tonal" href="#/tuning/flm">Analyze driving logs</a>
        </div>
        <p v-if="status === 'loading'" role="status">Reading local recordings…</p>
        <p v-if="status === 'unavailable'" role="alert">{{ error }}</p>
        <template v-if="status === 'ready' && data">
          <p v-if="data.scanIncomplete" role="status">This scan was incomplete. More local segments may exist.</p>
          <p v-if="!data.routes.length" role="status">No recording segments found in this scan.</p>
          <section v-if="playing" ref="playerPanel" class="gx-card gx-recordings__player" aria-label="Quick Road Video player">
            <div class="gx-recordings__player-head"><div><h3>Quick Road Video</h3><p class="gx-note">Lower-resolution local road view · {{ playing.routeId }} · segment {{ playing.segments[playing.index].number }}</p></div>
              <button type="button" class="gx-btn gx-btn--tonal" @click="closePlayer">Close</button></div>
            <video :key="playerGeneration" ref="quickVideo" :data-player-generation="playerGeneration" controls playsinline preload="metadata" :src="playing.url" @error="videoError($event)"></video>
            <p v-if="playerError" role="alert">{{ playerError }}</p>
            <div class="gx-recordings__player-controls"><button type="button" class="gx-btn gx-btn--tonal" :disabled="playing.index === 0" @click="chooseSegment(-1)">Previous segment</button>
              <span>{{ playing.index + 1 }} of {{ playing.segments.length }} saved Quick road segments</span>
              <button type="button" class="gx-btn gx-btn--tonal" :disabled="playing.index === playing.segments.length - 1" @click="chooseSegment(1)">Next segment</button></div>
          </section>
          <section v-if="detailsName" ref="detailsPanel" class="gx-card gx-recordings__details" aria-label="Recorded segment details">
            <div class="gx-recordings__player-head"><h3>Recorded Segment Details</h3><button type="button" class="gx-btn gx-btn--tonal" @click="closeDetails">Close</button></div>
            <p class="gx-note">{{ detailsName }} · For this saved segment only. Distance is estimated from recorded speed.</p>
            <p v-if="detailsStatus === 'loading'" role="status">Reading closed full log…</p>
            <p v-if="detailsStatus === 'unavailable'" role="alert">{{ detailsError }}</p>
            <div v-if="detailsStatus === 'ready' && details" class="gx-recordings__metrics">
              <div><strong>{{ detailMetric(details.observedCarSpanSeconds) }}</strong><span>Recorded time span</span></div>
              <div><strong>{{ detailMetric(details.estimatedDistanceMeters, 'm') }}</strong><span>Estimated distance</span></div>
              <div><strong>{{ detailMetric(details.observedLatActiveSeconds) }}</strong><span>Steering active</span></div>
              <div><strong>{{ detailMetric(details.observedLongActiveSeconds) }}</strong><span>Longitudinal active</span></div>
            </div>
            <p v-if="detailsStatus === 'ready' && details && !details.sampleCoverageComplete" class="gx-note">Samples have gaps or a missing source; unavailable values were not inferred.</p>
          </section>
          <section v-for="route in routeCards" :key="route.routeId" class="gx-card gx-recordings__route">
            <header><div><p class="gx-recordings__date"><time v-if="route.date.datetime" :datetime="route.date.datetime">{{ route.date.label }}</time><span v-else>{{ route.date.label }}</span><small v-if="route.date.source">{{ route.date.source }}</small></p><h3>{{ route.routeId }}</h3></div>
              <span class="gx-recordings__count">{{ route.segmentCount }} saved {{ route.segmentCount === 1 ? 'segment' : 'segments' }}</span></header>
            <ul class="gx-recordings__files" aria-label="Files saved for this drive"><li v-for="file in route.fileLabels" :key="file">{{ file }}</li></ul>
            <div class="gx-recordings__route-actions"><button v-if="route.firstQuick" type="button" class="gx-btn gx-btn--tonal" @click="openPlayer(route, route.firstQuick)"><i class="bi bi-play-fill" aria-hidden="true"></i>Play Quick video</button>
              <a v-if="route.connect" class="gx-btn gx-btn--tonal" :href="route.connect" target="_blank" rel="noopener noreferrer">Open in comma connect<i class="bi bi-box-arrow-up-right" aria-hidden="true"></i></a></div>
            <details class="gx-recordings__expand"><summary><span>View {{ route.segmentCount }} saved {{ route.segmentCount === 1 ? 'segment' : 'segments' }}</span><span class="gx-note">Playback, downloads &amp; details</span></summary>
            <ul class="gx-recordings__segments"><li v-for="segment in route.segments" :key="segment.number">
              <div class="gx-recordings__segment-info"><strong>Segment {{ segment.number }}</strong>
                <ul class="gx-recordings__files" aria-label="Saved files"><li v-for="file in availableFiles(segment.files)" :key="file">{{ file }}</li></ul>
              </div>
              <div class="gx-recordings__actions">
                <button v-if="segment.files.qcamera" type="button" class="gx-btn gx-btn--tonal" @click="openPlayer(route, segment)">Play video</button>
                <a v-if="segment.files.qcamera" class="gx-btn gx-btn--tonal" :href="quickRoadUrl(segment.segmentName)" :download="segment.segmentName + '-quick-road.mp4'">Download Quick video</a>
                <button v-if="segment.files.rlog" type="button" class="gx-btn gx-btn--tonal" @click="openDetails(segment)">Details</button>
              </div>
            </li></ul></details>
          </section>
        </template>
      </template>
    </div>`,
}
