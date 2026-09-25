import { navigate } from "./router.js"

export const Tools = {
  name: "Tools",
  props: { tools: { type: Array, required: true }, mode: { type: String, required: true } },
  methods: { open(tool) { navigate(tool.path) } },
  template: `
    <div>
      <h2 style="margin-top:0">Tools</h2>
      <div class="gx-grid">
        <button v-for="tool in tools" :key="tool.path" type="button" class="gx-tile" @click="open(tool)">
          <i class="bi" :class="tool.icon" aria-hidden="true"></i>
          <span>{{ tool.name }}</span>
          <small>{{ tool.description }}</small>
          <small v-if="mode !== 'local'" class="gx-availability">{{ tool.availability === 'partial-preview' ? 'Sample available' : 'Connect to your device to use this tool' }}</small>
        </button>
      </div>
    </div>
  `,
}
