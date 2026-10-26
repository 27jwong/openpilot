"""Speed group geometry shared by rendering, input and layout metadata."""

WIDTH = 488
MAX_ROW_HEIGHT = 132
LARGE_SIGN_SCALE = 1.25
LIMIT_ROW_HEIGHT = 168
CARD_HEIGHT = LIMIT_ROW_HEIGHT
ACTION_WIDTH = 308
ACTION_HEIGHT = 168
ACTION_X = 112
ACTION_Y = 267
GAP = 8
SOURCE_ROW_HEIGHT = 76
PADDING = 24

LIMIT_RECTS = {'large': (272, 75, 148, 165), 'compact': (330, 20, 118, 132)}
ACTION_BOUNDS = {
  'large': ((112, ACTION_Y, 420, ACTION_Y + 80), (112, ACTION_Y + 88, 420, ACTION_Y + ACTION_HEIGHT)),
  'compact': ((146, 44, 330, 102), (146, 106, 330, 164)),
}
ACTION_TEXT_SIZE = {'large': 64, 'compact': 48}


def card_height(mode):
  return CARD_HEIGHT if mode == 'split' else LIMIT_ROW_HEIGHT if mode == 'limit_only' else MAX_ROW_HEIGHT


def sources_extent(document):
  """Extra height the cruise widget takes for speed-limit sources: a row in the card, or a small card of its own below."""
  if document.get('speedSources') is not True:
    return 0
  return GAP + SOURCE_ROW_HEIGHT if document.get('speedSourcesCard') is True else SOURCE_ROW_HEIGHT
