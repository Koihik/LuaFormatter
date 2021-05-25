self.buf, self.win, self.buf_closer = floatbuf({
    win_width = self.rect.width,
    win_height = self.rect.height,
    x = self.rect.pos_x,
    y = self.rect.pos_y,
    loc = loc,
    prompt = self.prompt,
    enter = opts.enter,
    ft = opts.ft,
    syntax = opts.syntax,
    relative = opts.relative
  })