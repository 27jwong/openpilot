const { chromium } = require(process.env.PLAYWRIGHT_MODULE || 'playwright')
const assert = require('node:assert/strict')

;(async () => {
  const browser = await chromium.launch({ headless: true })
  const page = await browser.newPage({ deviceScaleFactor: 2, hasTouch: true })
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
      epoch: 1, start: 0, end: 120, position: 60, paused: true, rate: 1, seekRanges: [[0, 70], [80, 120]], sourceWarnings: [] }
    const legend = Object.fromEntries([
      ['chill', 'Chill', [22, 127, 64]], ['experimental', 'Experimental', [218, 111, 37]], ['aol', 'AOL', [10, 186, 181]],
      ['error', 'Alert', [201, 34, 49]], ['bookmark', 'Bookmark', [255, 214, 0]],
    ].map(([kind, label, rgb]) => [kind, { label, rgb }]))
    const history = new ReplayHistory(ownerId, sessionId)
    history.append({ schemaVersion: 1, ownerId, sessionId, rowOffset: 0, totalRows: 7, nextCursor: null, source: 'qlog', legend,
      rows: [[0, 20, 'chill'], [20, 40, 'aol'], [40, 70, 'experimental'], [55, 56, 'error'],
        [55.5, 55.5, 'bookmark'], [80, 120, 'chill'], [100, 100, 'bookmark']] }, status)
    document.body.innerHTML = '<main class="gx-content"><div id="fixture"></div></main>'
    window.replayControls = []
    window.fixtureVM = createApp({ ...ReplayViewer,
      data: () => ({ ...ReplayViewer.data(), replay: status, replayTimeline: markRaw(history), replayVideoReady: true,
        client: markRaw({ ownedSession: sessionId, control(...args) { window.replayControls.push(args) } }) }),
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
      return canvas?.height === (width <= 600 ? 44 : 54)
    }, width)
    const geometry = await page.locator('.gx-replay__track').evaluate(track => {
      const canvas = track.querySelector('canvas'), input = track.querySelector('input')
      const bounds = element => ({ width: element.getBoundingClientRect().width, height: element.getBoundingClientRect().height })
      return { canvas: bounds(canvas), input: bounds(input), pixels: canvas.width, overflow: document.documentElement.scrollWidth > innerWidth }
    })
    assert.equal(geometry.canvas.height, width <= 600 ? 22 : 27)
    assert.ok(geometry.input.height >= 44, 'the rail keeps its generous seek hit area')
    assert.ok(Math.abs(geometry.pixels - geometry.canvas.width * 2) <= 1)
    assert.equal(geometry.overflow, false)
    const slider = page.getByRole('slider', { name: 'Replay route position' })
    const box = await slider.boundingBox()
    const x = box.x + box.width * .5, y = box.y + box.height / 2
    const bitmap = await page.locator('.gx-replay__rail canvas').evaluate(canvas => canvas.toDataURL())
    await page.evaluate(() => {
      window.replayControls = []; window.replayPaints = 0
      const ctx = document.querySelector('.gx-replay__rail canvas').getContext('2d')
      if (!ctx.originalFillRect) {
        ctx.originalFillRect = ctx.fillRect
        ctx.fillRect = function (...args) { window.replayPaints++; this.originalFillRect(...args) }
      }
    })
    await page.mouse.move(x, y)
    await page.mouse.down()
    await page.mouse.move(box.x + box.width * .55, y + 25)
    const preview = page.locator('.gx-replay__preview')
    await preview.waitFor()
    assert.match(await preview.textContent(), /Experimental/)
    assert.equal(await page.evaluate(() => window.replayControls.length), 0, 'dragging only previews')
    const bounds = await preview.boundingBox()
    assert.ok(bounds.y + bounds.height < box.y - 20, 'preview stays above the touch target')
    assert.equal(await page.locator('.gx-replay__rail canvas').evaluate(canvas => canvas.toDataURL()), bitmap, 'scrubbing reuses the canvas')
    assert.equal(await page.evaluate(() => window.replayPaints), 0, 'scrubbing never repaints the history')
    if (process.env.REPLAY_SCREENSHOT_DIR) await page.screenshot({ path: `${process.env.REPLAY_SCREENSHOT_DIR}/replay-web-scrub-${width}.png` })
    await page.mouse.up()
    assert.equal(await preview.count(), 0)
    assert.equal(await page.evaluate(() => window.replayControls.length), 1, 'release commits exactly once')
    for (const fraction of [0, 1]) {
      await page.mouse.move(box.x + 8 + (box.width - 16) * fraction, y)
      await page.mouse.down()
      const edge = await preview.boundingBox()
      const player = await page.locator('.gx-replay__player').boundingBox()
      assert.ok(edge.x >= player.x && edge.x + edge.width <= player.x + player.width, 'edge preview stays inside player')
      const head = await page.locator('.gx-replay__playhead').boundingBox()
      const stem = await page.locator('.gx-replay__preview-line').boundingBox()
      assert.ok(Math.abs(stem.x - (head.x + head.width / 2)) <= 1, 'stem still points to the target when the pill is clamped')
      await page.mouse.up()
    }
    for (const cancel of ['escape', 'epoch', 'session', 'offroad']) {
      await page.evaluate(() => { window.replayControls = [] })
      await page.mouse.move(x, y); await page.mouse.down()
      if (cancel === 'escape') await page.keyboard.press('Escape')
      else await page.evaluate(cancel => {
        const vm = window.fixtureVM
        if (cancel === 'epoch') vm.replay = { ...vm.replay, epoch: vm.replay.epoch + 1 }
        if (cancel === 'session') {
          const sessionId = vm.replay.sessionId === 'b'.repeat(32) ? 'd'.repeat(32) : 'b'.repeat(32)
          vm.client.ownedSession = sessionId
          vm.replay = { ...vm.replay, sessionId }
        }
        if (cancel === 'offroad') vm.replay = { ...vm.replay, offroad: false }
      }, cancel)
      await page.mouse.move(box.x + box.width * .6, y)
      await page.mouse.up()
      assert.equal(await preview.count(), 0, `${cancel} clears the preview`)
      assert.equal(await page.evaluate(() => window.replayControls.length), 0, `${cancel} cancels the seek`)
      await page.evaluate(() => { window.fixtureVM.replay = { ...window.fixtureVM.replay, offroad: true } })
    }
    await slider.focus()
    await page.keyboard.press('ArrowRight')
    assert.match(await slider.getAttribute('aria-valuetext'), /Experimental/)
    await page.locator('summary', { hasText: 'Timeline colors' }).click()
    assert.equal(await page.locator('.gx-replay__legend span').count(), 5)
    if (process.env.REPLAY_SCREENSHOT_DIR) await page.screenshot({ path: `${process.env.REPLAY_SCREENSHOT_DIR}/replay-web-${width}.png` })
    await page.locator('summary', { hasText: 'Timeline colors' }).click()
    await page.getByRole('button', { name: 'Fullscreen video' }).click()
    await page.waitForFunction(() => document.fullscreenElement &&
      document.querySelector('.gx-replay__track canvas').height === (innerWidth <= 600 ? 44 : 54))
    const fullscreenHeight = await page.locator('.gx-replay__track canvas').evaluate(canvas => ({
      actual: canvas.getBoundingClientRect().height, expected: innerWidth <= 600 ? 22 : 27,
    }))
    assert.equal(fullscreenHeight.actual, fullscreenHeight.expected, 'fullscreen preserves restrained rail sizing')
    if (process.env.REPLAY_SCREENSHOT_DIR) await page.screenshot({ path: `${process.env.REPLAY_SCREENSHOT_DIR}/replay-web-fullscreen-${width}.png` })
    await page.evaluate(() => document.exitFullscreen())
  }
  await page.setViewportSize({ width: 390, height: 900 })
  const slider = page.getByRole('slider', { name: 'Replay route position' })
  const box = await slider.boundingBox(), cdp = await page.context().newCDPSession(page)
  for (const cancel of [false, true]) {
    await page.evaluate(() => { window.replayControls = [] })
    await cdp.send('Input.dispatchTouchEvent', { type: 'touchStart', touchPoints: [{ x: box.x + box.width * .5, y: box.y + box.height / 2 }] })
    await cdp.send('Input.dispatchTouchEvent', { type: 'touchMove', touchPoints: [{ x: box.x + box.width * .6, y: box.y + box.height / 2 + 20 }] })
    assert.equal(await page.locator('.gx-replay__preview').count(), 1, 'touch drag displays the raised preview')
    assert.equal(await page.evaluate(() => window.replayControls.length), 0)
    await cdp.send('Input.dispatchTouchEvent', { type: cancel ? 'touchCancel' : 'touchEnd', touchPoints: [] })
    assert.equal(await page.locator('.gx-replay__preview').count(), 0)
    assert.equal(await page.evaluate(() => window.replayControls.length), cancel ? 0 : 1, 'touch end commits once; cancellation never commits')
  }
  assert.deepEqual(errors, [])
  await browser.close()
  console.log('Replay timeline browser: sizing, cached drag preview, edges, touch, cancellation, authority changes, fullscreen and keyboard passed')
})().catch(error => { console.error(error); process.exit(1) })
