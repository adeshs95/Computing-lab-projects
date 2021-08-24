import ply.lex as lex
import ply.yacc as yacc

result = {'movie name' : [],'director' : [],'storyline' : [],'cast with character name' : [],'box office collection': [],'runtime': [], 'producer': [] , 'original language' : [] , 'writers' : []}

#To filter the detected groups for cast field
def filter_name(name_grp):	

	charac_name = ""
	i = 0
	while i < len(name_grp):
		if name_grp[i] != '\n' and name_grp[i] != ' ':

			charac_name += name_grp[i]
			i += 1
			continue

		if name_grp[i] == ' ' or name_grp[i] == '\n':
			while i < len(name_grp) and (name_grp[i] == ' ' or name_grp[i] == '\n') :
				i += 1

			
			if i < len(name_grp):

				if charac_name[-1].isalpha():
					charac_name += ' '

				charac_name += name_grp[i]
				
				i += 1

	return charac_name

tokens = ('LMOVIE','RMOVIE',
		  'RDIR',
		  'RWRITER',
		  'LPRODUCER','RPRODUCER',
		  'LLANGUAGE','RLANGUAGE',
		  'LSTORYLINE',
		  'LCOLLECTION','RCOLLECTION',
		  'LRUNTIME','RRUNTIME',
		  'RCAST','CAST',
		  'NAME',
		 )

def t_LMOVIE(t):
	r'<meta\ property=\"og:title\"\ content=\"'
	return t

def t_RMOVIE(t):
	r'\">'
	return t

def t_RDIR(t):
	r'\s*<br\/>\s*Director\s*<\/span>'
	return t

def t_RWRITER(t):
	r'\s*<br\/>\s*(Screenwriter|Writer)\s*<\/span>'   
	#print(t.lexer.lexmatch.groups())                            
	return t

def t_LPRODUCER(t):
	r'<span\ class=\"characters\ subtle\ smaller\"\ title=\"'

	return t

def t_RPRODUCER(t):
	r'\s*<br\/>\s*Producer\s*<\/span>'
	return t

def t_LLANGUAGE(t):
	r'<div\ class=\".+\">Original\ Language.+\n.+>'
	return t

def t_RLANGUAGE(t):
	r'\n[\ ]+<\/div>'
	return t

def t_LSTORYLINE(t):
	r'<meta\ property=\"og:description\"\ content=\"([^\n><]*)\">'
	
	t.value = t.lexer.lexmatch.group(11)
	return t

def t_LCOLLECTION(t):
	r'<div\ class=\".+\">Box\ Office\ \(Gross\ USA\):.+\n.+data-qa=\"movie-info-item-value\">'

	return t

def t_RCOLLECTION(t):
	r'<\/div>'
	return t

def t_LRUNTIME(t):
	r'<div\ .+>Runtime\:.+\n.+\n.+>\n[\ ]+'
	return t

def t_RRUNTIME(t):
	r'\n[\ ]+<\/time>'
	return t

def t_CAST(t):
	r'\s*<br\/>\s*(Self)*\s*<\/span>'
	#print(t.lexer.lexmatch.groups())
	sname = t.lexer.lexmatch.group(17)
	if sname == None:
		sname = ""

	t.value = sname
	return t

def t_RCAST(t):
	r'\s*<br\/>\s*([^\\\/><]*)\s*<br\/>\s*([^\\\/><]*)\s*<\/span>'    
	
	#t.value = t.lexer.lexmatch.groups() 
	#print(t.lexer.lexmatch.groups())

	name_grp1 = t.lexer.lexmatch.group(19)
	name_grp2 = t.lexer.lexmatch.group(20) 
	
	charac_name1 = filter_name(name_grp1)
	charac_name2 = filter_name(name_grp2)
	t.value = ""

	if charac_name1 != "":
		t.value += charac_name1 

	if charac_name2 != "":
		t.value += " " + charac_name2

	#print(t.lexer.lexmatch.group(16) + t.lexer.lexmatch.group(17))
	return t

def t_NAME(t):
	r'[^\\\/>\"<=\n]+'
	return t

def t_error(t):
	t.lexer.skip(1)

def p_start(t):
	'''start : movie
			 | director
			 | writer
			 | producer
			 | collection
			 | language
			 | runtime
			 | storyline
			 | cast		 
	'''
	pass 
	

def p_movie(t):
	'movie : LMOVIE names RMOVIE'
	result['movie name'].append(t[2])

def p_director(t):
	'director : LPRODUCER names RMOVIE RDIR'
	result['director'].append(t[2])

def p_writer(t):
	'writer : LPRODUCER names RMOVIE RWRITER'
	result['writers'].append(t[2])

def p_producer(t):
	'producer : LPRODUCER names RMOVIE RPRODUCER'
	result['producer'].append(t[2])
	

def p_collection(t):
	'collection : LCOLLECTION names RCOLLECTION'
	result['box office collection'].append(t[2])

def p_language(t):
	'language : LLANGUAGE names RLANGUAGE'
	result['original language'].append(t[2])

def p_runtime(t):
	'runtime : LRUNTIME names RRUNTIME'
	result['runtime'].append(t[2])

def p_storyline(t):
	'storyline : LSTORYLINE'
	result['storyline'].append(t[1])
	

def p_cast(t):
	'''cast : LPRODUCER names RMOVIE RCAST
			| LPRODUCER names RMOVIE CAST
	'''
			
	
	if t[2] == "":
		result['cast with character name'].append("     ")
	else:	
		result['cast with character name'].append(t[2])

		if t[4] == "":
			result['cast with character name'][-1] += "()"
		else:
			result['cast with character name'][-1] += "(" + t[4] + ")"
	
def p_names(t):
	'names : NAME'
	t[0] = t[1]



def p_error(t):
	pass
