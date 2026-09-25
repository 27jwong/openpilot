import { reactive } from "../vendor/vue/vue.esm-browser.js"

// This preview deliberately has no compatibility iframe or device endpoint.
const start = () => {
  const hash = location.hash.slice(1) || "/"
  try { return decodeURIComponent(hash) } catch { return hash }
}
export const route = reactive({ path: start() })

export function navigate(path) {
  route.path = path
  location.hash = encodeURI(path)
  window.scrollTo(0, 0)
}

export function startRouter() {
  if (!location.hash) location.replace(`${location.pathname}${location.search}#/`)
  window.addEventListener("hashchange", () => {
    route.path = start()
    window.scrollTo(0, 0)
  })
}
