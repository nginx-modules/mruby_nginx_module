static const char *ngx_http_mruby_string_pcre_rb_string = 
"class String\n"
"  alias_method :old_sub, :sub\n"
"  def sub(*args, &blk)\n"
"    if args[0].class == String\n"
"      return blk ? old_sub(*args) { |x| blk.call(x) } : old_sub(*args)\n"
"    end\n"
"\n"
"    begin\n"
"      m = args[0].match(self)\n"
"    rescue\n"
"      return self\n"
"    end\n"
"    return self if m.size == 0\n"
"    r = ''\n"
"    r += m.pre_match\n"
"    r += blk ? blk.call(m[0]) : args[1]\n"
"    r.to_sub_replacement!(m)\n"
"    r += m.post_match\n"
"    r\n"
"  end\n"
"\n"
"  alias_method :old_gsub, :gsub\n"
"  def gsub(*args, &blk)\n"
"    if args[0].class.to_s == 'String'\n"
"      return blk ? old_gsub(*args) { |x| blk.call(x) } : old_gsub(*args)\n"
"    end\n"
"\n"
"    s = self\n"
"    r = \"\"\n"
"    while true\n"
"      begin\n"
"        m = args[0].match(s)\n"
"      rescue\n"
"        break\n"
"      end\n"
"\n"
"      break if !m || m.size == 0\n"
"      return r if m.end(0) == 0\n"
"      r += m.pre_match\n"
"      r += blk ? blk.call(m[0]) : args[1]\n"
"      r.to_sub_replacement!(m)\n"
"      s = m.post_match\n"
"    end\n"
"    r += s\n"
"    r\n"
"  end\n"
"\n"
"  def =~(a)\n"
"    begin\n"
"      (a.class.to_s == 'String' ? Regexp.new(a.to_s) : a) =~ self\n"
"    rescue\n"
"      false\n"
"    end\n"
"  end\n"
"\n"
"  alias_method :old_split, :split\n"
"  def split(*args, &blk)\n"
"    return [] if self.empty?\n"
"\n"
"    if args[0].nil? or args[0].class.to_s == 'String'\n"
"      return blk ? old_split(*args) { |x| blk.call(x) } : old_split(*args)\n"
"    end\n"
"\n"
"    if args.size < 2\n"
"      limited = false\n"
"      limit = 0\n"
"    else\n"
"      limit = args[1].to_i\n"
"\n"
"      if limit > 0\n"
"        return [self.dup] if limit == 1\n"
"        limited = true\n"
"      else\n"
"        tail_empty = true\n"
"        limited = false\n"
"      end\n"
"    end\n"
"\n"
"    pattern = args[0]\n"
"    result = []\n"
"    # case '//'\n"
"    if pattern.source.empty?\n"
"      index = 0\n"
"      while true\n"
"        break if limited and limit - result.size <= 1\n"
"        break if index + 1 >= self.length\n"
"\n"
"        result << self[index]\n"
"        index += 1\n"
"      end\n"
"      result << self[index..-1]\n"
"    else\n"
"      start = 0\n"
"      last_match = nil\n"
"      last_match_end = 0\n"
"\n"
"      while m = pattern.match(self, start)\n"
"        break if limited and limit - result.size <= 1\n"
"\n"
"        unless m[0].empty? and (m.begin(0) == last_match_end)\n"
"          result << m.pre_match[last_match_end..-1]\n"
"          result.push(*m.captures)\n"
"        end\n"
"        \n"
"        if m[0].empty?\n"
"          start += 1\n"
"        elsif last_match and last_match[0].empty?\n"
"          start = m.end(0) + 1\n"
"        else\n"
"          start = m.end(0)\n"
"        end\n"
"\n"
"        last_match = m\n"
"        last_match_end = m.end(0) || 0\n"
"\n"
"        break if self.length <= start \n"
"      end\n"
"\n"
"      if last_match\n"
"        result << last_match.post_match\n"
"      elsif result.empty?\n"
"        result << self.dup\n"
"      end\n"
"    end\n"
"\n"
"    # Trim (not specified in the second argument)\n"
"    if !result.empty? and (limit.nil? || limit == 0)\n"
"      while result.last.nil? or result.last.empty?\n"
"        result.pop\n"
"      end\n"
"    end\n"
"\n"
"    result\n"
"  end\n"
"\n"
"  alias_method :old_slice, :slice\n"
"  alias_method :old_square_brancket, :[]\n"
"\n"
"  def [](*args)\n"
"    return old_square_brancket(*args) unless args[0].class == Regexp\n"
"\n"
"    if args.size == 2\n"
"      match = args[0].match(self)\n"
"      if match\n"
"        if args[1] == 0\n"
"          str = match[0]\n"
"        else\n"
"          str = match.captures[args[1] - 1]\n"
"        end\n"
"        return str\n"
"      end\n"
"    end\n"
"\n"
"    match_data = args[0].match(self)\n"
"    if match_data\n"
"      result = match_data.to_s\n"
"      return result\n"
"    end\n"
"  end\n"
"\n"
"  alias_method :slice, :[]\n"
"  # XXX: alias_method :old_slice!, :slice!\n"
"  def slice!(*args)\n"
"    if args.size < 2\n"
"      result = slice(*args)\n"
"      nth = args[0]\n"
"\n"
"      if nth.class == Regexp\n"
"        lm = Regexp.last_match\n"
"        self[nth] = '' if result\n"
"        Regexp.last_match = lm\n"
"      else\n"
"        self[nth] = '' if result\n"
"      end\n"
"    else\n"
"      result = slice(*args)\n"
"\n"
"      nth = args[0]\n"
"      len = args[1]\n"
"\n"
"      if nth.class == Regexp\n"
"        lm = Regexp.last_match\n"
"        self[nth, len] = '' if result\n"
"        Regexp.last_match = lm\n"
"      else\n"
"        self[nth, len] = '' if result\n"
"      end\n"
"    end\n"
"\n"
"    result\n"
"  end\n"
"\n"
"  # private\n"
"  def to_sub_replacement!(match)\n"
"    result = \"\"\n"
"    index = 0\n"
"    while index < self.length\n"
"      current = index\n"
"      while current < self.length && self[current] != '\\\\'\n"
"        current += 1\n"
"      end\n"
"      result += self[index, (current - index)]\n"
"      break if current == self.length\n"
"\n"
"      if current == self.length - 1\n"
"        result += '\\\\'\n"
"        break\n"
"      end\n"
"      index = current + 1\n"
"\n"
"      cap = self[index]\n"
"\n"
"      case cap\n"
"      when \"&\"\n"
"        result += match[0]\n"
"      when \"`\"\n"
"        result += match.pre_match\n"
"      when \"'\"\n"
"        result += match.post_match\n"
"      when \"+\"\n"
"        result += match.captures.compact[-1].to_s\n"
"      when /[0-9]/\n"
"        result += match[cap.to_i].to_s\n"
"      when '\\\\'\n"
"        result += '\\\\'\n"
"      else\n"
"        result += '\\\\' + cap\n"
"      end\n"
"      index += 1\n"
"    end\n"
"    self.replace(result)\n"
"  end\n"
"\n"
"  alias_method :old_scan, :scan\n"
"  def scan(*args, &blk)\n"
"    return old_scan(*args) if args[0].class == String\n"
"\n"
"    s = self\n"
"    ret = []\n"
"    last_match = nil\n"
"    while m = args[0].match(s)\n"
"      break if !m || m.size == 0\n"
"      return ret if m.end(0) == 0\n"
"\n"
"      val = (m.size == 1 ? m[0] : m.captures)\n"
"      s = m.post_match\n"
"\n"
"      if blk\n"
"        blk.call(val)\n"
"      else\n"
"        ret << val\n"
"      end\n"
"    end\n"
"\n"
"    ret\n"
"  end\n"
"\n"
"\n"
"  #\n"
"  # XXX: Need pull-request to http://github.com/mruby/mruby mrbgems/mruby-string-ext\n"
"  #\n"
"  def []=(*args)\n"
"    index = args[0]\n"
"    if args.size != 3\n"
"      val = args[1]\n"
"      count = nil   \n"
"    else\n"
"      count = args[1]\n"
"      val = args[2]\n"
"    end\n"
"\n"
"    case index\n"
"    when Fixnum\n"
"      index += self.size if index < 0\n"
"\n"
"      raise IndexError, \"index #{index} out of string\" if index < 0 or index > self.size\n"
"      raise IndexError, \"unable to find charactor at: #{index}\" unless bi = index\n"
"\n"
"      if count\n"
"        count = count.to_i\n"
"        raise IndexError, \"count is negative\" if count < 0\n"
"\n"
"        total = index + count\n"
"        bs = total - bi\n"
"      else\n"
"        bs = index == size ? 0 : (index + 1) - bi\n"
"      end\n"
"\n"
"      splice bi, bs, val\n"
"    when String\n"
"      raise IndexError, \"string not matched\" unless start = self.index(index)\n"
"\n"
"      splice start, index.size, val\n"
"    when Range\n"
"      start = index.first.to_i\n"
"      start += self.size if start < 0\n"
"\n"
"      raise RangeError, \"#{index.first} is out of range\" unless bi = start\n"
"      stop = index.last.to_i\n"
"      stop += self.size if stop < 0\n"
"      stop -= 1 if index.exclude_end?\n"
"\n"
"      if stop < start\n"
"        bs = 0\n"
"      else\n"
"        bs =  stop + 1 - bi\n"
"      end\n"
"\n"
"      splice bi, bs, val\n"
"    when Regexp\n"
"      count = count || 0\n"
"\n"
"      if match = index.match(self)\n"
"        ms = match.size\n"
"      else\n"
"        raise IndexError, \"regexp does not match\"\n"
"      end\n"
"\n"
"      count += ms if count < 0 and -count < ms\n"
"      raise IndexError, \"index #{count} out of match bounds\" unless count < ms and count >= 0\n"
"\n"
"      bi = match.begin(count)\n"
"      bs = match.end(count) - bi\n"
"\n"
"      splice bi, bs, val\n"
"    else\n"
"      index = index.to_i\n"
"\n"
"      if count\n"
"        return self[index, count] = val\n"
"      else\n"
"        return self[index] = val\n"
"      end\n"
"    end\n"
"\n"
"    return val\n"
"  end\n"
"\n"
"  def splice(start , count, val)\n"
"    self.replace(self[0...start] + val + self[(start + count)..-1])\n"
"  end\n"
"end\n"
;
;
