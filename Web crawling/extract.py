import urllib.request, urllib.error, urllib.parse
import os
import sys
import re
import ply.lex as lex
import ply.yacc as yacc
from parsers import *

store = []	#Will contain the requested field values
def main():
	
	exist =0

	while True:
		genre = input("Enter the name of genre :")

		if genre !="Action &Adventure" and genre !="Animation" and genre !="Drama" and genre !="Comedy" and genre !="Mystery & Suspense"and genre !="Horror"and genre !="Sci-Fi"and genre !="Documentary"and genre !="Romance"and genre !="Classics":
			print("Invalid genre name !!!")
			print()
			continue
		else:
			break


	dir_name = "data"			#will contain all downloaded movie and genre html pages.
	if not os.path.exists(dir_name):
		os.mkdir(dir_name)

	else:
		for file_name in os.listdir(dir_name):
			if file_name == genre + ".html":
				exist = 1
				break

	if not exist:
		count = 0
		f =  open('rottentomatoes movie genre link.txt', 'r')
		Lines = f.readlines()

		for line in Lines:

			if count %2 !=0 and g == genre:
				response = urllib.request.urlopen(line.strip())
				webContent = response.read()

				f = open("data/" + genre + ".html", 'wb')
				f.write(webContent)
				f.close()
				print(genre + ".html is Downlaoded ")
				break

			else:
				g = line.strip()

			count += 1

	print(genre + ".html is Downlaoded ")

	f = open("data/" + genre + ".html" , 'r')
	lines = f.read()
	f.close()

	movies_name = re.findall(r"<td>\n\        <a href=\"(.+)\" class=\"unstyled articleLink\">\n            (.+)<\/a>", lines)

	for m in movies_name:
		print(m[1])

	print()

	while True :

		for m in movies_name:
			print(m[1])

		print()

		movie = input("Enter the movie name :")
		exist = 0
		flag = 0

		for file_name in os.listdir(dir_name):
			if file_name == movie + ".html":
				exist = 1
				break

		if not exist:
			for m in movies_name:
				if m[1] == movie:
					
					response = urllib.request.urlopen("https://www.rottentomatoes.com" + m[0] + "/")
					
					webContent = response.read()
					
					print("movie is downloaded")

					f = open("data/" + movie + ".html", 'wb')
					f.write(webContent)
					flag = 1
					f.close()	
					break

			if not flag:
				print("Invalid movie name!!!")
				print()
				continue

		lexer = lex.lex()
		parser = yacc.yacc()

		f = open("data/" + movie + ".html", 'r')
		text = f.read()
		parser.parse(text)
		
		
		while True:
			field = input("Enter the field :")
			
			if field.lower() not in result.keys():
				print("Please enter the valid field!!!")
				print()
				continue

			print()
			for r in result[field.lower()]:

				print(r)
				store.append([genre , movie , field , r])
				print()

			print()
			f1 = open("log.txt" , 'a')
			widths = [max(map(len, col)) for col in zip(*store)] #Just prettify

			for s in store:
					
				f1.write("  ".join((val.ljust(width) for val, width in zip(s, widths)))) # result[movie name][0] + "\t" +     #
				f1.write("\n")
				f1.write("\n")


			f1.close()
			choice =  ""

			while True:
				choice = input("Do you want to check another field?[y/n]: ")

				if choice == 'n':
					break
				elif choice == 'y':
					break
				else:
					print("Invalid choice!!!")

			if choice == 'n':
				break

		while True:
			choice = input("Do you want to check another movie?[y/n]: ")

			if choice == 'n':
				return
			elif choice == 'y':
				break
			else:
				print("Invalid choice!!!")

main()

