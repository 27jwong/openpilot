// Read-only local crash reports. State is cleared on navigation and sign-out.
export class CrashReportsFeed {
  constructor({ publish, unauthorized = () => {}, fetcher = (...args) => fetch(...args) }) {
    Object.assign(this, { publish, unauthorized, fetcher })
    this.active = false
    this.generation = 0
    this.request = null
    this.reports = []
  }

  clear() {
    this.reports = []
    this.publish({ reports: [], scanIncomplete: false, listLimited: false, status: "idle", error: "", selected: null, preview: null, previewStatus: "idle" })
  }

  stop() {
    this.active = false
    this.generation++
    this.request?.abort()
    this.request = null
    this.clear()
  }

  start() {
    this.stop()
    this.active = true
    return this.load()
  }

  async requestJson(url, generation, request) {
    const response = await this.fetcher(url, { signal: request.signal, cache: "no-store" })
    if (!this.active || generation !== this.generation || request.signal.aborted) return null
    if (response.status === 401) {
      this.stop()
      this.unauthorized()
      return null
    }
    if (response.status === 503) {
      const body = await response.json().catch(() => null)
      if (!this.active || generation !== this.generation || request.signal.aborted) return null
      if (["access_unavailable", "setup_required"].includes(body?.code)) {
        this.stop()
        this.unauthorized()
        return null
      }
      throw new Error("Crash reports are unavailable")
    }
    if (!response.ok) throw new Error(response.status === 409 ? "Report changed; refresh the list" : "Report unavailable")
    const body = await response.json()
    if (!this.active || generation !== this.generation || request.signal.aborted) return null
    return body
  }

  async load() {
    if (!this.active) return
    const generation = ++this.generation
    this.request?.abort()
    const request = new AbortController()
    this.request = request
    this.reports = []
    this.publish({ reports: [], scanIncomplete: false, listLimited: false, status: "loading", error: "", selected: null, preview: null, previewStatus: "idle" })
    try {
      const data = await this.requestJson("./api/crash-reports", generation, request)
      if (!data) return
      if (data.schemaVersion !== 1 || !Array.isArray(data.reports) || data.reports.length > 200 ||
          typeof data.scanIncomplete !== "boolean" || typeof data.listLimited !== "boolean" ||
          data.reports.some((item) => typeof item.id !== "string" || !item.id ||
            typeof item.name !== "string" || !item.name ||
            typeof item.size !== "number" || !Number.isFinite(item.size) || item.size < 0 ||
            typeof item.modifiedAt !== "number" || !Number.isFinite(item.modifiedAt))) {
        throw new Error("Invalid crash report list")
      }
      this.reports = data.reports
      this.publish({ reports: data.reports, scanIncomplete: data.scanIncomplete, listLimited: data.listLimited, status: "ready", error: "", selected: null, preview: null, previewStatus: "idle" })
    } catch (error) {
      if (this.active && generation === this.generation) this.publish({ reports: [], scanIncomplete: false, listLimited: false, status: "unavailable", error: error.message, selected: null, preview: null, previewStatus: "idle" })
    } finally {
      if (generation === this.generation) this.request = null
    }
  }

  async open(report) {
    if (!this.active || !this.reports.some((item) => item.id === report?.id)) return
    const generation = ++this.generation
    this.request?.abort()
    const request = new AbortController()
    this.request = request
    this.publish({ selected: report.id, preview: null, previewStatus: "loading", error: "" })
    try {
      const data = await this.requestJson(`./api/crash-reports/${encodeURIComponent(report.id)}`, generation, request)
      if (!data) return
      if (data.schemaVersion !== 1 || data.name !== report.name || typeof data.text !== "string" || typeof data.truncated !== "boolean") {
        throw new Error("Invalid crash report")
      }
      this.publish({ selected: report.id, preview: data, previewStatus: "ready", error: "" })
    } catch (error) {
      if (this.active && generation === this.generation) this.publish({ selected: null, preview: null, previewStatus: "unavailable", error: error.message })
    } finally {
      if (generation === this.generation) this.request = null
    }
  }
}
