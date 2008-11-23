#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# parser/parse_mashou.py
# Copyright (C) 2008 John Eric Martin <john.eric.martin@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the
# Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

import BeautifulSoup, demjson, random, time, udc
import ajax, urllib, urllib2, codecs, sys, re, os

def strip_tags(value):
	return re.sub(r"<[^>]*?>", u"", value)

def cache_file(url, post, filename):
	if not os.path.exists(filename):
		print u"Downloading file '%s'..." % filename

		# Open the url
		page = urllib2.urlopen(url, urllib.urlencode(post))

		# Save the url to the file
		handle = open(filename, "w")
		handle.write( page.read() )
		handle.close()

		# Sleep for 1-5 seconds
		time.sleep(random.uniform(1000000, 5000000) / 1000000)

	# Open the file
	handle = open(filename, "r")
	page = handle.read().decode("euc-jp-udc")
	handle.close()

	return page

def parse_mashou(page, ajax_inst, done = 1, total = 1):
	soup = BeautifulSoup.BeautifulSoup(page, fromEncoding="utf-8")

	row = { }

	info = soup.findAll(name=u"td",
		attrs={u"class":u"viewM01", u"bgcolor":u"FFF0FF"}, limit=3)
	if len(info) != 3:
		print u"Error parsing mashou."
		sys.exit()

	name = info[0].findAll(name=u"b", limit=1)

	if len(name) != 1:
		print u"Error parsing mashou name."
		sys.exit()

	difficulty = name[0].nextSibling.strip()
	match = re.compile(u"難易度：([0-9]+)").match(difficulty)
	if not match:
		print u"Error parsing mashou difficulty."
		sys.exit()

	try:
		difficulty = int( match.group(1) )
	except ValueError:
		print u"Error parsing mashou difficulty."
		sys.exit()

	name = name[0].renderContents(encoding=None).strip()

	fusion1 = info[1].findAll(name=u"b", limit=1)
	if len(fusion1) != 1:
		print u"Error parsing mashou info."
		sys.exit()

	fusion1 = fusion1[0].nextSibling.strip().split(u"　　")
	level = fusion1[0].split(u"〜")
	if len(fusion1) > 1:
		fusion1 = fusion1[1]
	else:
		fusion1 = u"N/A"

	try:
		row[u"fusion1_min"] = int(level[0])
	except ValueError:
		row[u"fusion1_min"] = -1

	try:
		row[u"fusion1_max"] = int(level[1])
	except ValueError:
		row[u"fusion1_max"] = -1

	fusion2 = info[2].findAll(name=u"font", limit=1)
	if len(fusion2) != 1:
		print u"Error parsing mashou info."
		sys.exit()

	fusion2_info = fusion2[0].findAll(name=u"b", limit=3)
	if len(fusion2_info) != 3:
		print u"Error parsing mashou info."
		sys.exit()

	level = fusion2_info[0].nextSibling.strip().split(u"〜")
	equip_type = fusion2_info[1].nextSibling.strip()
	name_modifier = fusion2_info[2].nextSibling.strip()

	try:
		row[u"fusion2_min"] = int(level[0])
	except ValueError:
		row[u"fusion2_min"] = -1

	try:
		row[u"fusion2_max"] = int(level[1])
	except ValueError:
		row[u"fusion2_max"] = -1

	fusion2 = fusion2[0].findAll(name=u"br", limit=1)
	if len(fusion2) != 1:
		print u"Error parsing mashou info."
		sys.exit()

	fusion2 = fusion2[0].nextSibling.strip()

	row[u"name_ja"] = name
	row[u"name_en"] = u"Untitled"
	row[u"fusion1_desc_ja"] = fusion1 # mayosooi
	row[u"fusion2_desc_ja"] = fusion2 # tamagou
	row[u"fusion1_desc_en"] = u"Write me!"
	row[u"fusion2_desc_en"] = u"Write me!"
	row[u"equip_type"] = ajax_inst.handle_relation(u"db_equip_type",
		equip_type, u"Untitled")
	row[u"name_modifier_ja"] = name_modifier
	row[u"name_modifier_en"] = u"Untitled"
	row[u"difficulty"] = difficulty
	row[u"icon"] = u"I71_0004a"

	action = { u"action":u"insert", u"table":u"db_mashou",
		u"columns":row.keys(), u"user_data":u"parse_mashou_insert",
		u"rows":[row] }

	result = ajax_inst.request(action)[0]
	if result.has_key(u"error"):
		print u"Error adding mashou to the database:\n%s" % result[u"error"]
		print u"Original JSON request sent was:\n%s" % demjson.encode(action)
		sys.exit()
	else:
		print u"Mashou '%s' added to the database at row %d (%d of %d)" \
			% ( name, result[u"ids"][0], done, total )

# ---------
# Main Code
# ---------

random.seed()
if not os.path.exists("cache"):
	os.mkdir("cache")

ajax_inst = ajax.ajax()
ajax_inst.login()

print u""
print u"Finding the mashou list..."

url = "http://artifact.sakura.ne.jp/imagine/"

keys = ["chkbox12", "chkbox2", "chkbox31", "chkbox32", "chkbox33",
	"chkbox34", "chkbox35", "chkbox36", "chkbox37", "chkbox38", "chkbox39",
	"data1_1_old", "data1_2_old", "data1_3_old", "data1_4_old", "data2_1",
	"data2_1_old", "data2_2_old", "data2_3_old", "data2_4_old", "data3_1_old",
	"data3_2_old", "data3_3_old", "data3_4_old", "data4_1_old", "data4_2_old",
	"data4_3_old", "data4_4_old", "data5_1_old", "data5_2_old", "data5_3_old",
	"data5_4_old", "data6_1_old", "data6_2_old", "data6_3_old", "data6_4_old",
	"data7_1_old", "data7_2_old", "data7_3_old", "data7_4_old", "data7_5_old",
	"data7_6_old", "devil_lv", "hist", "iden_flag", "old_view", "slct1",
	"slct1Label", "state", "state_old", "tree_list"]

post = { }
for key in keys:
	post[key] = ""

post["state"] = 5

page = cache_file(url, post, "cache/mashou_index.html")
soup = BeautifulSoup.BeautifulSoup(page)

# Get the select option for showing the mashou list of all mashou
selects = soup.findAll(name=u"select", attrs={u"name":u"data6_1"})
if len(selects) != 1:
	print u"Error parsing mashou list."
	sys.exit()

data6_1 = [ ]
for tag in selects[0].findAll(name=u"option"):
	value = tag["value"].strip()
	if value:
		data6_1.append(value)

if len(data6_1) < 1:
	print u"Error parsing mashou list."
	sys.exit()

# Should be '魔晶一覧'
post["data6_1"] = data6_1[0].encode("euc-jp")
post["state_old"] = post["state"]

print u"Reading the mashou list..."

page = cache_file(url, post, "cache/mashou_list.html")
soup = BeautifulSoup.BeautifulSoup(page)

# Get the mashou ids (data6_2 values) from the list of mashou
selects = soup.findAll(name=u"select", attrs={u"name":u"data6_2"})
if len(selects) != 1:
	print u"Error parsing mashou ids."
	sys.exit()

data6_2 = [ ]
for tag in selects[0].findAll(name=u"option", attrs={u"value":True}):
	value = tag["value"].strip()
	if value:
		data6_2.append(value)

post["data6_1_old"] = post["data6_1"]

print u"Parsing the mashou..."

done = 0
total = len(data6_2)

# Loop over all the codes and parse the mashou
for code in data6_2:
	done += 1

	# Set the filename
	filename = u"cache/%s.html" % code

	# Set the POST data
	post["data6_2"] = code.encode("euc-jp")

	# Get the file
	page = cache_file(url, post, filename)

	# Parse the mashou
	parse_mashou(page, ajax_inst, done, total)

print u"Parsing done! Enjoy your mashou!!!"
