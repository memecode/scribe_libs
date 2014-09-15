import os
import sys
import ftplib
import urlparse

start = os.getcwd()

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
		if not os.path.exists(out_file):
			uri = urlparse.urlparse(url)
			path = uri.path
			parts = path.rsplit("/", 1)
			print parts
			
			# download the file from 'url' to 'out_file'
			print "    opening FTP connection..."
			ftp = ftplib.FTP("ftp.gnu.org")
			ftp.login()
			print "    changing to", parts[0]
			ftp.cwd(parts[0])
			print "    getting", parts[1]
			ftp.retrbinary(parts[1], open(out_file, "wb").write)
			ftp.quit()
