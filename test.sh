make -C libft >/dev/null
gcc -Wall -Wextra -Werror -fsanitize=address -g3 srcs/*.c -Iincs -Ilibft/incs libft/libft.a -o minishell
./minishell
# rm minishell