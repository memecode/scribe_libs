import os
import sys
import urlparse
import urllib

start = os.getcwd()

ftp = None

def download_cb(block):
	file.write(block)

def invalid_file(name):
	return not os.path.exists(name) or os.stat(name).st_size == 0

txt = open("AvailableDictionaries.html", "r").read().split("<tr>")
for line in txt:
	p = line.split("<td>")
	if len(p[0]) == 0:
		del p[0]
	if len(p) > 3:
		lang = p[0].split('\"')[1]
		eng_name = p[1]
		native_name = ["", p[2]][p[2].find("&nbsp;")]
		url = "ftp://ftp.gnu.org/gnu/aspell/dict/" + p[3].split('\"')[1]
		data = [lang, eng_name, native_name, url]
		print ", ".join(data)
		
		leaf = url.split("/")[-1]
		
		out_path = os.path.join(start, lang)
		if not os.path.exists(out_path):
			os.mkdir(out_path)
		
		out_file = os.path.join(out_path, leaf)
		if invalid_file(out_file):
			# download the file from 'url' to 'out_file'
			urllib.urlretrieve(url, filename=out_file)
		
		if not invalid_file(out_file):
			# unpack the files
			os.chdir(out_path)
			cmd = "\"C:\\Program Files\\7-Zip\\7z.exe\" e -y \""+out_file+"\""
			os.system("\"" + cmd + "\"")
			
			tar_name = out_file.rsplit(".", 1)[0]
			cmd = "\"C:\\Program Files\\7-Zip\\7z.exe\" e -y \""+tar_name+"\""
			os.system("\"" + cmd + "\"")
			os.unlink(tar_name)

		# build and package dictionary