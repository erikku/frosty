#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# parser/ajax.py
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

import StringIO, hashlib, urllib, pycurl, demjson, getpass, sys, re

class ajax:
	__login_ok = False
	__backend_url = "https://gigadelic.homelinux.net:55517/backend.php"
	__email = ""
	__password = ""
	__salt = ""
	__connection = pycurl.Curl()

	def __process_headers(self, data):
		headerMatcher = re.compile(r"^([^:]+): (.*)$")
		headers = { }

		for i in data.split(u"\r\n"):
			match = headerMatcher.match(i)
			if match:
				headers[match.group(1).strip()] = match.group(2).strip()

		return headers

	def __get_salt(self):
		json = self.request({u"action":u"salt"})[0]
		if json.has_key(u"error"):
			print u"Error fetching authentication salt:\n%s" % json[u"error"]
			return

		self.__salt = json[u"salt"].encode("ascii")

	def request(self, action):
		response_content = StringIO.StringIO()
		response_headers = StringIO.StringIO()

		# Check the action argument is valid (to some extent)
		if type(action) == dict:
			post = { "request" : demjson.encode({u"actions":[action]},
				escape_unicode=True, encoding="ascii") }
		elif type(action) == list:
			post = { "request" : demjson.encode({u"actions":action},
				escape_unicode=True, encoding="ascii") }
		else:
			return [{u"error":u"The argument is not an object or array."}]

		# Handle the authentication
		if self.__salt and self.__email and self.__password:
			post["email"] = self.__email

			pass_hash = hashlib.sha1()
			pass_hash.update(self.__salt)
			pass_hash.update(self.__password)
			pass_hash = pass_hash.hexdigest()

			request_hash = hashlib.sha1()
			request_hash.update(pass_hash)
			request_hash.update(post["request"])
			request_hash = request_hash.hexdigest()

			post["pass"] = request_hash

		post = urllib.urlencode(post)

		# Make the request
		self.__connection.setopt(pycurl.URL, self.__backend_url)
		self.__connection.setopt(pycurl.HEADERFUNCTION, response_headers.write)
		self.__connection.setopt(pycurl.WRITEFUNCTION, response_content.write)
		self.__connection.setopt(pycurl.CAINFO, "../client/res/ca.crt")
		self.__connection.setopt(pycurl.SSLVERSION, pycurl.SSLVERSION_SSLv3)
		self.__connection.setopt(pycurl.POSTFIELDS, post)
		self.__connection.setopt(pycurl.POST, 1)
		self.__connection.perform()

		# Get the response headers
		headers = unicode(response_headers.getvalue(), "utf-8")
		headers = self.__process_headers(headers)
		if headers.has_key(u"X-JSON") == False:
			return [{u"error":u"X-JSON header missing."}]

		response_code = self.__connection.getinfo(pycurl.HTTP_CODE)
		if response_code != 200:
			return [{u"error":u"Server returned HTTP response code %d"
				u"but 200 expected." % response_code}]

		response_content = unicode(response_content.getvalue(), "utf-8")
		if response_content:
			return [{u"error":u"Unexpected content found in message "
				u"body:\n%s" % response_content}]

		# Decode the JSON
		json_match = re.compile(r"^\((.+)\)$").match(headers[u"X-JSON"])
		if json_match == False:
			return [{u"error":u"Error parsing X-JSON header."}]

		# Check that the JSON decoded right
		try:
			json = demjson.decode( json_match.group(1) )
		except demjson.JSONError, e:
			return [{u"error":u"JSON decode error: %s" % e.pretty_description()}]

		# Make sure the response makes sense
		if type(json) != list:
			return [{u"error":u"Server response should be an array."}]
		if len(json) < 1:
			return [{u"error":u"Server response should have at least 1 item."}]

		return json

	def login(self, backend_url = False, email = False, password = False,
		prompt = True):

		if self.__login_ok:
			return

		if prompt:
			print u"Please enter the following info (leaving an entry " \
				u"blank will use the value shown in brackets)"
			print u""

		default_url = backend_url if backend_url else self.__backend_url
		default_password = password if password else self.__password
		default_email = email if email else self.__email

		if not prompt:
			self.__get_salt()
			self.__backend_url = default_url
			self.__password = default_password
			self.__email = default_email
			self.__login_ok = True
			return

		# Backend URL
		sys.stdout.write(u"Backend URL [%s]: " % default_url)
		temp = sys.stdin.readline().strip()
		self.__backend_url = temp if temp else default_url

		# Get the password salt
		self.__get_salt()

		# Email
		sys.stdout.write(u"Email [%s]: " % default_email)
		temp = sys.stdin.readline().strip()
		self.__email = temp if temp else default_email

		# Password
		temp = getpass.getpass(u"Password: ").strip()
		self.__password = temp if temp else default_password

		self.__login_ok = True

	def request_id(self, table, column, value):
		action = { u"action":u"select", u"table":table, u"columns":[u"id"],
			u"where":[{u"column":column, u"type":u"equals", u"value":value}],
			u"where_type":u"and", u"limit":1 }

		result = self.request(action);
		result = result[0];

		if result.has_key(u"error"):
			return -1

		rows = result[u"rows"]
		if len(rows) != 1:
			return -1

		return rows[0][u"id"]

	def update_relation(self, table, id_value, name_ja, name_en):
		action = { u"action":u"update", u"table":table, u"columns":[u"name_ja",
			u"name_en"], u"where":[{u"column":"id", u"type":u"equals",
			u"value":id_value}], u"where_type":"and",
			u"rows":[{u"name_ja":name_ja, u"name_en":name_en}] }

		return self.request(action);

	def insert_relation(self, table, name_ja, name_en):
		action = {u"action":u"insert", u"table":table,u"columns":[u"name_ja",
			u"name_en"], u"rows":[{u"name_ja":name_ja, u"name_en":name_en}] }

		return self.request(action);

	def handle_relation(self, table, name_ja, name_en, update=False,
		insert=True):
		id_value = self.request_id(table, u"name_ja", name_ja);
		if id_value == -1 and insert == True:
			# TODO: Error check this
			resp = self.insert_relation(table, name_ja, name_en)
			return resp[0][u"ids"][0];
			return -1
		elif update:
			# TODO: Error check this
			self.update_relation(table, id_value, name_ja, name_en)
			return id_value
		else:
			return id_value
