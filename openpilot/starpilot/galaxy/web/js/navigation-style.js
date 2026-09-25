if (typeof document !== "undefined") {
  const style = document.createElement("style")
  style.textContent = `
.gx-navigation--fullscreen {position:fixed;inset:0;z-index:15;padding:0!important;background:#0e0e1a;overflow:hidden}
.gx-app:has(.gx-navigation--fullscreen) .gx-appbar {background:none;box-shadow:none;pointer-events:none;transform:none}
.gx-app:has(.gx-navigation--fullscreen) .gx-appbar__pill,.gx-app:has(.gx-navigation--fullscreen) .gx-theme-toggle {display:none}
.gx-app:has(.gx-navigation--fullscreen) .gx-appbar__back,.gx-app:has(.gx-navigation--fullscreen) .gx-appbar__menu {position:fixed;top:auto;bottom:16px;pointer-events:auto;background:#161630;border:1px solid #343453;border-radius:12px}
.gx-app:has(.gx-navigation--fullscreen) .gx-appbar__back {left:auto;right:72px}
.gx-app:has(.gx-navigation--fullscreen) .gx-appbar__menu {right:16px}
@media(min-width:768px) {.gx-nav-pinned .gx-navigation--fullscreen {left:320px}}
.gx-navigation--fullscreen>h2 {display:none}
.gx-navigation--fullscreen>.gx-navigation__tabs {position:absolute;bottom:16px;left:16px;z-index:4;margin:0;gap:8px}
.gx-navigation--fullscreen .gx-navigation-map {position:absolute;inset:0;margin:0;border-radius:0;background:#0e0e1a}
.gx-navigation--fullscreen .gx-navigation-map canvas {height:100%;background:#0e0e1a}
.gx-navigation__panel {position:absolute;top:16px;left:16px;width:30%;max-height:calc(100% - 88px);overflow:auto;z-index:3}
.gx-navigation__panel .gx-card {background:#0e0e1a;border:0;border-radius:16px;box-shadow:0 4px 12px #0005}
.gx-navigation__panel .gx-navigation__section {max-width:500px;margin:8px 0;padding:8px}
.gx-navigation__panel .gx-btn,.gx-navigation__tabs .gx-btn {border:1px solid #343453;border-radius:16px;background:#161630;box-shadow:0 2px 6px #0004;font-size:.85rem;min-height:44px;padding:8px 16px}
.gx-navigation__search {display:flex;flex-wrap:nowrap;gap:8px;margin:0 0 8px}
.gx-navigation__search .gx-field {min-width:0;padding:16px;border-radius:16px;border:1px solid #343453;background:#161630;font-size:.85rem;height:49px}
.gx-navigation__search .gx-btn {padding:8px;flex-shrink:0;min-width:112px;height:49px}
.gx-navigation__summary-title {margin:0 0 8px;padding:0;border-radius:12px;background:#161630;text-align:center;font-size:1.2rem;line-height:1.2}
.gx-navigation__summary>div {display:grid;grid-template-columns:32px 96px 1fr;align-items:center;font-size:1rem;font-weight:550;min-height:32px}
.gx-navigation__summary>div>span:first-child {text-align:center}
.gx-navigation__route-actions {display:flex;justify-content:center;gap:8px;margin-top:8px}
.gx-navigation__route-actions .gx-btn {background:#e05577;border:0;border-radius:16px;flex:1;color:#e8e8f0;font-weight:550;letter-spacing:1px}
.gx-navigation__panel .gx-navigation__instruction {font-size:1rem;margin:8px 0}
.gx-navigation__panel .gx-note {font-size:.85rem}
.gx-navigation__places {gap:8px;margin:8px 0}
.gx-navigation__places>li {padding:16px;border:1px solid #343453!important;background:#161630!important;border-radius:12px!important;gap:8px;font-size:.85rem}
.gx-navigation__places .gx-note {display:block;font-size:.75rem}
.gx-navigation__actions {gap:8px}
.gx-navigation__alternatives {display:flex;flex-wrap:wrap;gap:8px;margin:8px 0}
.gx-navigation-map__last {position:absolute;bottom:48px;right:16px;background:#0e0e1ae8;border-radius:12px;padding:8px 12px;font-size:12px;color:#ddd}
.gx-navigation--fullscreen .gx-navigation-map__controls {top:16px;right:16px;flex-direction:column}
.gx-navigation-map__controls .gx-btn {background:#161630;border:1px solid #343453;border-radius:12px;box-shadow:0 2px 6px #0004}
.gx-navigation-map > p {position:absolute;bottom:72px;right:16px;max-width:320px;background:#0e0e1a;border-radius:12px;padding:12px;font-size:12px}
.gx-navigation__tabs button {white-space:nowrap}
@media(max-width:768px) and (orientation:portrait) {
.gx-navigation__panel {left:8px;top:16px;width:calc(100% - 32px);max-height:55%}
.gx-navigation--fullscreen .gx-navigation-map__controls {top:auto;bottom:92px}
.gx-navigation__tabs .gx-btn {font-size:12px;padding:8px}
.gx-navigation__places>li {flex-direction:row;align-items:center}
}
`
  document.head.appendChild(style)
}
