make -C libft >/dev/null
gcc -Wall -Wextra -Werror srcs/*.c -Iincs -Ilibft/incs libft/libft.a -o minishell
./minishell
# rm minishell