#!/bin/sh
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    run_test.sh                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: besellem <besellem@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/30 11:31:57 by besellem          #+#    #+#              #
#    Updated: 2021/05/30 12:20:24 by besellem         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

### Colors
RED="\033[0;31m"
GREEN="\033[0;32m"
YLW="\033[0;33m"
BLUE="\033[0;34m"
PURPLE="\033[0;35m"
GRAY="\033[0;37m"
B_RED="\033[1;31m"
B_GREEN="\033[1;32m"
B_YLW="\033[1;33m"
B_BLUE="\033[1;34m"
B_PURPLE="\033[1;35m"
B_GRAY="\033[1;37m"
B_CYAN="\033[1;36m"
CLR_COLOR="\033[0m"
CLR_LINE="\033[2K\r"
CLR_SCREEN="\033[2J\033[H"

### Global constants
__NAME__="run_test.sh"
__SHL_TST_PATH__="."
__LOG_FILE__="$__SHL_TST_PATH__/results.log"

### Global variables
i=1
errors=0


### Print Header
print_header() {
	echo "${CLR_SCREEN}${B_RED}\
   __  ____      _     __       ______          __         
  /  |/  (_)__  (_)__ / /  ___ / / / /____ ___ / /____ ____
 / /|_/ / / _ \/ (_-</ _ \/ -_) / / __/ -_|_-</ __/ -_) __/
/_/  /_/_/_//_/_/___/_//_/\__/_/_/\__/\__/___/\__/\__/_/   \
${CLR_COLOR}\n
    ${B_YLW}Credits to besellem ${CLR_COLOR}\n"
}


### Clean tmp .log file
clean() {
	ls $__LOG_FILE__ > /dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "🛠  ${B_YLW}Deleting ${__LOG_FILE__}...${CLR_COLOR} \c"
		rm -f $__LOG_FILE__
		echo "✅ "
	fi
}


### Clean tmp .log file
fclean() {
	echo "🛠  ${B_YLW}make fclean...${CLR_COLOR} \c"
	make fclean > /dev/null 2>&1
	echo "✅ "
	clean
}


### Print usage
print_usage() {
	clean
	# echo
	echo "usage: ./$__NAME__ [option]"
	echo
	echo "Options available:"
	echo "\`${B_YLW}-c${CLR_COLOR}' or \`${B_YLW}clean${CLR_COLOR}':   Delete log file"
	echo "\`${B_YLW}-f${CLR_COLOR}' or \`${B_YLW}fclean${CLR_COLOR}':  Clean all project"
	echo "\`${B_YLW}-h${CLR_COLOR}' or \`${B_YLW}help${CLR_COLOR}':    Print usage"
	echo "\`${B_YLW}-a${CLR_COLOR}' or \`${B_YLW}all${CLR_COLOR}':     Do all tests"
	echo "\`${B_YLW}-p${CLR_COLOR}' or \`${B_YLW}parsing${CLR_COLOR}': Do parsing tests"
	echo "\`${B_YLW}-r${CLR_COLOR}' or \`${B_YLW}redir${CLR_COLOR}':   Do redirections tests"
}


### Compile and check if it was done properly 
init_tester() {

	# Check if the $__SHL_TST_PATH__ exist
	if [ ! -d "$__SHL_TST_PATH__" ]; then
		mkdir $__SHL_TST_PATH__
	fi

	# Compile
	echo "🛠  ${B_YLW}Compiling your project...${CLR_COLOR} \c"
	make >/dev/null 2>&1
	if [ `ls minishell` != "minishell" ]; then
		echo
		echo "${B_RED}Error:${CLR_COLOR} Check your Makefile"
	else
		echo "✅ "
	fi
}


### Print nb of errors and successes
print_stats() {
	echo
	echo "You got ${B_RED}${errors}${CLR_COLOR} errors on ${B_YLW}${i}${CLR_COLOR} tests\c"
	if [ $# -ge 1 ]; then
		echo " on your ${1}"
	else
		echo
	fi
	if [ $errors -gt 0 ]; then
		echo "Check ${__LOG_FILE__} to see your errors"
	fi
}


### General command exec & test function
test_cmd() {

	printf "Test #%2d " $i
	# echo "Test #${i} \c"

	# Make test (redirect all output to the file)
	echo $1 | bash > $__SHL_TST_PATH__/real 2>&1
	echo $1 | ./minishell > $__SHL_TST_PATH__/mine 2>&1

	# Add diff between two files in a `diffs' file
	diff $__SHL_TST_PATH__/real $__SHL_TST_PATH__/mine > $__SHL_TST_PATH__/diffs

	# Add diffs in the .log file if any
	if [ $? -ne 0 ]; then
		echo "❌ => [${1}]"	# debug purpose
		# echo "❌  "
		echo "Test #${i}: [$1]" >> $__SHL_TST_PATH__/results.log
		cat $__SHL_TST_PATH__/diffs >> $__SHL_TST_PATH__/results.log
		echo >> $__SHL_TST_PATH__/results.log
		errors=$((errors+1))
	else
		echo "✅ => [${1}]"	# debug purpose
		# echo "✅  "
	fi

	# echo "[${1}]"	# debug purpose
	
	# Delete tmp files
	rm -f $__SHL_TST_PATH__/diffs $__SHL_TST_PATH__/real $__SHL_TST_PATH__/mine
	
	# Increment count index
	i=$((i+1))
}


### Do parser tests
test_parser() {

	tmp="$__SHL_TST_PATH__/try_empty"
	echo | ./minishell > $tmp 2>&1
	
	if [ `cat $tmp | wc -c | xargs; rm -f $tmp` != "0" ]; then
		echo ${B_RED}
		echo "Your shell must not print its prompt when passing it through stdin"
		echo "Try this command:              ${B_YLW}echo | ./minishell${B_RED}"
		echo "Then compare it with this one: ${B_YLW}echo | bash${B_RED}"
		echo "You'll see that minishell is printing your prompt but not bash"
		echo "This is because you must print the prompt on STDIN (fd = 0)"
		echo ${CLR_COLOR}
		exit 1
	fi

	echo
	echo "${B_CYAN}# Testing your parser...${CLR_COLOR}"

	# (re)init global variables
	errors=0
	i=1

	# Tests
	test_cmd ""
	test_cmd "       "
	test_cmd "    		 "
	test_cmd "echo"
	test_cmd "echo "
	test_cmd " echo"
	test_cmd "   	echo 		"
	test_cmd "echo ''"
	test_cmd "echo bonjour"
	test_cmd "echo       bonjour       "
	test_cmd "echo \\"
	test_cmd "echo \\"
	test_cmd "echo \\\\"
	test_cmd "echo \\\\\\"
	test_cmd "ech\"o\" test"
	test_cmd "\\ec\\h\\o"
	test_cmd "echo \"\'a\""
	test_cmd "echo '\"a'"
	test_cmd "echo \\\"'\"a'"
	# test_cmd "echo \"\'a\""
	test_cmd "echo \"\\\"a\""
	test_cmd "echo \"bonjour\""
	test_cmd "echo \"''bonjour\""
	test_cmd "echo '' \"''bonjour\"\"\""
	test_cmd "echo ''\"''bonjour\"''"
	test_cmd "echo \\\"\"''\\\"\"\"bonjour\""
	test_cmd "echo \"a\"\"b\""
	test_cmd "echo \"\\ \""
	test_cmd "echo \"\\\\\\\\ \""
	test_cmd "echo '\\'"
	test_cmd "echo '\\\\\\\\'"
	test_cmd "echo \$"
	test_cmd "echo '\$'"
	test_cmd "echo \"\$\""
	test_cmd "echo \"\$?\""
	test_cmd "echo \"\$DOES_NOT_EXIST\""
	test_cmd "echo \"\$HOME\""
	test_cmd "echo '\$HOME'"
	test_cmd "echo \"'\$HOME'\""
	test_cmd "echo \"Bonjour\$HOME\\\toi\""
	test_cmd "echo \" bonjour '' 'a\' \$LESS\$BONJOUR\moi\"\\\"\'"
	test_cmd "echo ' bonjour \"\" \"a\\\" \$LESS\$BONJOUR\moi'\\\"\'"

	test_cmd "echo bonjour;"
	test_cmd "echo bonjour ;"
	test_cmd "echo bonjour ; "
	test_cmd "echo bonjour;grep ''"
	test_cmd "echo bonjour ;grep ''"
	test_cmd "echo bonjour; grep ''"
	test_cmd "echo bonjour ; grep ''"
	test_cmd "echo bonjour ; grep '' "
	test_cmd "echo;echo;echo;echo"
	test_cmd "cd ../../../../../.. ; pwd"
	test_cmd "cd ; pwd"
	test_cmd "cd . ; pwd"

	print_stats "parser"
}


### Do redirections tests
test_redirections() {

	echo
	# echo "${B_CYAN}# Testing your redirections...${CLR_COLOR}"
	echo "${B_RED}Error: Redirections tests are not yet implemented${CLR_COLOR}"

}

################################################################################
### Main
################################################################################
print_header
clean # delete old log file if exist

if [ $# -lt 1 ] || [ $1 = '-h' ] || [ $1 = 'help' ]; then
	print_usage
elif [ $1 = '-c' ] || [ $1 = 'clean' ]; then
	clean
elif [ $1 = '-f' ] || [ $1 = 'fclean' ]; then
	fclean
elif [ $1 = "-a" ] || [ $1 = "all" ]; then
	init_tester
	test_parser
	test_redirections
elif [ $1 = "-p" ] || [ $1 = "parser" ]; then
	init_tester
	test_parser
elif [ $1 = "-r" ] || [ $1 = "redir" ]; then
	init_tester
	test_redirections
else
	if [ $1 != "" ]; then
		echo "☠  \`$1': ${B_RED}Command not found${CLR_COLOR}\n"
	fi
	print_usage
fi
