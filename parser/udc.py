#!/usr/bin/python
# -*- coding: utf-8 -*-

import codecs

class UDC_Codec(codecs.Codec):
	def __init__(self):
		original_codec = codecs.lookup("euc_jp")

		self.encode_chart = {
			u"\u2170":"\xfc\xf1", u"\u2171":"\xfc\xf2", u"\u2172":"\xfc\xf3",
			u"\u2173":"\xfc\xf4", u"\u2174":"\xfc\xf5", u"\u2175":"\xfc\xf6",
			u"\u2176":"\xfc\xf7", u"\u2177":"\xfc\xf8", u"\u2178":"\xfc\xf9",
			u"\u2179":"\xfc\xfa", u"\u217a":"\xfc\xfb", u"\u217b":"\xfc\xfc" }
		self.encode_function = original_codec.encode

		self.decode_chart = {
			"\xfc\xf1":u"\u2170", "\xfc\xf2":u"\u2171", "\xfc\xf3":u"\u2172",
			"\xfc\xf4":u"\u2173", "\xfc\xf5":u"\u2174", "\xfc\xf6":u"\u2175",
			"\xfc\xf7":u"\u2176", "\xfc\xf8":u"\u2177", "\xfc\xf9":u"\u2178",
			"\xfc\xfa":u"\u2179", "\xfc\xfb":u"\u217a", "\xfc\xfc":u"\u217b" }
		self.decode_function = original_codec.decode

	def encode(self, seq, errors = 'strict'):
		out = str()
		temp = unicode()
		count = 0

		for i in seq:
			if self.encode_chart.has_key(i):
				if temp:
					try:
						ret = self.encode_function(temp, errors)
					except UnicodeEncodeError, e:
						raise UnicodeEncodeError("euc_jp_udc", seq,
							e.start + count, e.end + count, e.reason)

					out += ret[0]
					count += ret[1]

				out += self.encode_chart[i]
				count += 1

				temp = unicode()
			else:
				temp += i

		if temp:
			try:
				ret = self.encode_function(temp, errors)
			except UnicodeEncodeError, e:
				raise UnicodeEncodeError("euc_jp_udc", seq,
					e.start + count, e.end + count, e.reason)

			out += ret[0]
			count += ret[1]

		return (out, count)

	def decode(self, seq, errors = 'strict'):
		out = unicode()
		temp = seq
		count = 0

		index = 0
		while len(temp) >= 2 and index < len(temp):
			if self.decode_chart.has_key(temp[index:index+2]):
				if index > 0:
					try:
						ret = self.decode_function(temp[0:index], errors)
					except UnicodeDecodeError, e:
						raise UnicodeDecodeError("euc_jp_udc", seq,
							e.start + count, e.end + count, e.reason)

					out += ret[0]
					count += ret[1]

				out += self.decode_chart[ temp[index:index+2] ]
				count += 2

				temp = temp[index+2:]
				index = 0
			else:
				index += 1

		if temp:
			try:
				ret = self.decode_function(temp, errors)
			except UnicodeDecodeError, e:
				raise UnicodeDecodeError("euc_jp_udc", seq,
					e.start + count, e.end + count, e.reason)

			out += ret[0]
			count += ret[1]

		return (out, count)

def codec_eucjp_udc(encoding_name):
	defaults = codecs.lookup("euc_jp")

	return codecs.CodecInfo(
		name='euc_jp_udc',
		encode=UDC_Codec().encode,
		decode=UDC_Codec().decode,
        incrementalencoder=defaults.incrementalencoder,
        incrementaldecoder=defaults.incrementaldecoder,
        streamreader=defaults.streamreader,
        streamwriter=defaults.streamwriter,
      )

codecs.register(codec_eucjp_udc)
