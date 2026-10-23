const { chromium } = require(process.env.PLAYWRIGHT_MODULE || 'playwright')
const assert = require('node:assert/strict')

;(async () => {
  const browser = await chromium.launch({ headless: true })
  const page = await browser.newPage({ deviceScaleFactor: 2 })
  const errors = []
  page.on('pageerror', error => errors.push(error.message))
  await page.route('**/js/boot.js', route => route.fulfill({ body: '' }))
  await page.route('**/api/**', route => route.fulfill({ status: 404, body: '' }))
  await page.goto(process.env.GALAXY_URL || 'http://127.0.0.1:8765/')
  await page.evaluate(async () => {
    const { createApp, markRaw } = await import('./vendor/vue/vue.esm-browser.js')
    const { ReplayViewer } = await import('./js/replay.js')
    const { ReplayHistory } = await import('./js/replay-timeline.js')
    const ownerId = 'a'.repeat(32), sessionId = 'b'.repeat(32)
    const status = { schemaVersion: 1, ownerId, sessionId, state: 'paused', owned: true, offroad: true,
      start: 0, end: 120, position: 60, paused: true, rate: 1, seekRanges: [[0, 70], [80, 120]], sourceWarnings: [] }
    const legend = Object.fromEntries([
      ['chill', 'Chill', [22, 127, 64]], ['experimental', 'Experimental', [218, 111, 37]], ['aol', 'AOL', [10, 186, 181]],
      ['error', 'Alert', [201, 34, 49]], ['bookmark', 'Bookmark', [255, 214, 0]],
    ].map(([kind, label, rgb]) => [kind, { label, rgb }]))
    const history = new ReplayHistory(ownerId, sessionId)
    history.append({ schemaVersion: 1, ownerId, sessionId, rowOffset: 0, totalRows: 7, nextCursor: null, source: 'qlog', legend,
      rows: [[0, 20, 'chill'], [20, 40, 'aol'], [40, 70, 'experimental'], [55, 56, 'error'],
        [55.5, 55.5, 'bookmark'], [80, 120, 'chill'], [100, 100, 'bookmark']] }, status)
    document.body.innerHTML = '<main class="gx-content"><div id="fixture"></div></main>'
    window.fixtureVM = createApp({ ...ReplayViewer,
      data: () => ({ ...ReplayViewer.data(), replay: status, replayTimeline: markRaw(history), replayVideoReady: true,
        client: markRaw({ ownedSession: sessionId, control() {} }) }),
      mounted() {
        this.resizeRail = () => this.drawRail(true)
        this.railObserver = new ResizeObserver(this.resizeRail)
        this.railObserver.observe(this.$refs.player)
        window.addEventListener('resize', this.resizeRail)
        this.$nextTick(() => this.drawRail(true))
      },
      beforeUnmount() {},
    }, { clientId: 'c'.repeat(32), routeId: 'fixture', unauthorized() {} }).mount('#fixture')
  })
  for (const width of [1280, 390]) {
    await page.setViewportSize({ width, height: 900 })
    await page.waitForFunction(width => {
      const canvas = document.querySelector('.gx-replay__track canvas')
      return canvas?.height === (width <= 600 ? 26 : 32)
    }, width)
    const geometry = await page.locator('.gx-replay__track').evaluate(track => {
      const canvas = track.querySelector('canvas'), input = track.querySelector('input')
      const bounds = element => ({ width: element.getBoundingClientRect().width, height: element.getBoundingClientRect().height })
      return { canvas: bounds(canvas), input: bounds(input), pixels: canvas.width, overflow: document.documentElement.scrollWidth > innerWidth }
    })
    assert.equal(geometry.canvas.height, width <= 600 ? 13 : 16)
    assert.ok(geometry.input.height >= 44, 'the rail keeps its generous seek hit area')
    assert.ok(Math.abs(geometry.pixels - geometry.canvas.width * 2) <= 1)
    assert.equal(geometry.overflow, false)
    const slider = page.getByRole('slider', { name: 'Replay route position' })
    await slider.focus()
    await page.keyboard.press('ArrowRight')
    assert.match(await slider.getAttribute('aria-valuetext'), /Experimental/)
    await page.locator('summary', { hasText: 'Timeline colors' }).click()
    assert.equal(await page.locator('.gx-replay__legend span').count(), 5)
    if (process.env.REPLAY_SCREENSHOT_DIR) await page.screenshot({ path: `${process.env.REPLAY_SCREENSHOT_DIR}/replay-web-${width}.png` })
    await page.locator('summary', { hasText: 'Timeline colors' }).click()
    await page.getByRole('button', { name: 'Fullscreen video' }).click()
    await page.waitForFunction(() => document.fullscreenElement &&
      document.querySelector('.gx-replay__track canvas').height === (innerWidth <= 600 ? 26 : 32))
    const fullscreenHeight = await page.locator('.gx-replay__track canvas').evaluate(canvas => ({
      actual: canvas.getBoundingClientRect().height, expected: innerWidth <= 600 ? 13 : 16,
    }))
    assert.equal(fullscreenHeight.actual, fullscreenHeight.expected, 'fullscreen preserves restrained rail sizing')
    await page.evaluate(() => document.exitFullscreen())
  }
  assert.deepEqual(errors, [])
  await browser.close()
  console.log('Replay timeline browser: desktop/mobile/fullscreen rail heights, DPR, touch target, keyboard, labels and legend passed')
})().catch(error => { console.error(error); process.exit(1) })
