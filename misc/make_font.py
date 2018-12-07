import re
input_lines = []

with open('fixedc.fd.txt') as f:
	input_lines = [x.strip() for x in f.readlines()]

output_string = "const uint8_t font=[][13] = {\n"

char_rows = 0
for line in input_lines:
	if re.match("char [0-9]+",line):
		char_rows = 0
		output_string += '{'
	elif re.match("[01]{6}",line):
		bin_string = line + "00"
		val = int(bin_string,2)
		output_string+=format(val, '#04x')
		if char_rows < 12:
			output_string+=','
		else:
			output_string+='},\n'
		char_rows+=1
output_string+='};\n'		
print(output_string)

