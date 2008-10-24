#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# parser/parse_skills.py
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

import BeautifulSoup, demjson, random, time
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
	handle = codecs.open(filename, encoding="euc-jp")
	page = handle.read()
	handle.close()

	return page

def parse_skill(page, ajax_inst, done = 1, total = 1):
	soup = BeautifulSoup.BeautifulSoup(page, fromEncoding="utf-8")

	info = soup.findAll(name=u"td",
		attrs={u"class":u"viewS01", u"bgcolor":u"F0FFF0"})
	if len(info) != 2:
		print u"Error parsing skill."
		sys.exit()

	name = info[0].findAll(name=u"b", limit=1)
	if len(name) < 1:
		print u"Error parsing skill name."
		sys.exit()

	name = name[0].extract().renderContents(encoding=None).strip()

	elements = info[0].findAll(name=u"font", limit=1)
	if len(elements) < 1:
		print u"Error parsing skill."
		sys.exit()

	elements = elements[0].renderContents(encoding=None)
	elements = strip_tags( elements.replace(u"　", u" ").strip() )
	elements = elements.replace(u"相性:", u"相性 ")
	elements = elements.replace(u"コスト:", u"コスト ")
	elements = elements.replace(u"威力:", u"威力 ")
	elements = elements.replace(u"依存:", u"依存 ")
	elements = elements.replace(u"遺伝:", u"遺伝 ")
	elements = elements.strip().split(u" ")

	if len(elements) != 12:
		print u"Error parsing skill."
		sys.exit()

	desc = info[1].findAll(name=u"font", limit=1)
	if len(desc) < 1:
		print u"Error parsing skill description."
		sys.exit()

	desc = desc[0].renderContents(encoding=None).replace(u"　", u" ").strip()

	row = { u"hp_cost":0, u"mp_cost":0, u"mag_cost":0, u"icon":u"SB01_0001a",
		u"name_en":u"Untitled", u"class":-1, u"rank":-1, "player_only":0 }

	# Parse the skill cost
	for cost in elements[5].split(u"/"):
		match = re.compile(r"^HP([0-9]+)\%$").match(cost)
		if match:
			row[u"hp_cost"] = match.group(1)
			continue

		match = re.compile(r"^MP([0-9]+)$").match(cost)
		if match:
			row[u"mp_cost"] = match.group(1)
			continue

		match = re.compile(r"^MAG([0-9]+)$").match(cost)
		if match:
			row[u"mag_cost"] = match.group(1)

	row[u"name_ja"] = name
	row[u"desc_ja"] = desc
	row[u"desc_en"] = u"依存: %s" % elements[9]
	row[u"expert"] = ajax_inst.handle_relation(u"db_expert",
		u"不詳", u"Unknown")
	#row[u"expert"] = ajax_inst.handle_relation(u"db_expert",
	#	elements[9], u"Untitled")
	row[u"category"] = ajax_inst.handle_relation(u"db_category",
		elements[0], u"Untitled")
	row[u"action_type"] = ajax_inst.handle_relation(u"db_action_type",
		elements[1], u"Untitled")
	row[u"affinity"] = ajax_inst.handle_relation(u"db_affinity",
		elements[3], u"Untitled")

	row[u"power"] = -1
	try:
		row[u"power"] = int(elements[7])
	except:
		pass

	inherit = { }
	inherit[u"？"] = -2 # unknown
	inherit[u"遺伝不可"] = -1 # none
	inherit[u"ブレス"] = 0 # breath
	inherit[u"羽"] = 1 # wing
	inherit[u"突撃"] = 2 # pierce
	inherit[u"牙"] = 3 # fang
	inherit[u"爪"] = 4 # claw
	inherit[u"針"] = 5 # needle
	inherit[u"武器"] = 6 # sword
	inherit[u"乙女"] = 7 # strange (daughter?)
	inherit[u"目"] = 8 # special
	inherit[u"無制限"] = 99 # any

	if inherit.has_key(elements[11]):
		row[u"inheritance"] = inherit[ elements[11] ]
	else:
		print u"Unknown inheritance type '%s'." % elements[11]
		sys.exit()

	action = { u"action":u"insert", u"table":u"db_skills",
		u"columns":row.keys(), u"user_data":u"parse_skills_insert",
		u"rows":[row] }

	result = ajax_inst.request(action)[0]
	if result.has_key(u"error"):
		print u"Error adding skill to the database:\n%s" % result[u"error"]
		print u"Original JSON request sent was:\n%s" % demjson.encode(action)
		sys.exit()
	else:
		print u"Skill '%s' added to the database at row %d (%d of %d)" \
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
print u"Finding the skill list..."

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

post["state"] = 1

page = cache_file(url, post, "cache/index.html")
soup = BeautifulSoup.BeautifulSoup(page)

# Get the select option for showing the skill list of all skills sorted by name
selects = soup.findAll(name=u"select", attrs={u"name":u"data2_1"})
if len(selects) != 1:
	print u"Error parsing skill list."
	sys.exit()

data2_1 = [ ]
for tag in selects[0].findAll(name=u"option"):
	data2_1.append( tag.renderContents(encoding=None) )

if len(data2_1) < 1:
	print u"Error parsing skill list."
	sys.exit()

post["data2_1"] = data2_1[0].encode("euc-jp")
post["state_old"] = post["state"]

print u"Reading the skill list..."

page = cache_file(url, post, "cache/list.html")
soup = BeautifulSoup.BeautifulSoup(page)

# Get the skill ids (data2_2 values) from the list of skills
selects = soup.findAll(name=u"select", attrs={u"name":u"data2_2"})
if len(selects) != 1:
	print u"Error parsing skill ids."
	sys.exit()

data2_2 = [ ]
for tag in selects[0].findAll(name=u"option", attrs={u"value":True}):
	value = tag["value"].strip()
	if value:
		data2_2.append(value)

post["data2_1_old"] = post["data2_1"]

print u"Parsing the skills..."

done = 0
total = len(data2_2)

# Loop over all the codes and parse the skills
for code in data2_2:
	done += 1

	# Set the filename
	filename = u"cache/%s.html" % code

	# Set the POST data
	post["data2_2"] = code.encode("euc-jp")

	# Get the file
	page = cache_file(url, post, filename)

	# Parse the skill
	parse_skill(page, ajax_inst, done, total)

print u"Parsing done! Enjoy your skills!!!"
