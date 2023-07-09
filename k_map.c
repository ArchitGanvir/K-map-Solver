#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

int cca_2[] = {0, 2, 1, 3};
int cca_3[] = {0, 4, 1, 5, 3, 7, 2, 6};
int cca_4[] = {0, 4, 12, 8, 1, 5, 13, 9, 3, 7, 15, 11, 2, 6, 14, 10};
//cca is the cell coordinate array. cca[i] gives the coordinates of the i'th cell

int *cca_n[] = {cca_2, cca_3, cca_4}; //Array of pointers for possible values of cca

void print_minimized_exp(char *s);
void delete_spaces(char *s);
int num_of_var(char *s);
void using_k_map(char *s);
int* form_k_map(char *s);
char* form_var_char_arr(char *s);
int bin_to_dec_unsigned(int bin);
void prime_implicants(int *k_map, int n, char *v);
int left(int cc, int n);
int up(int cc, int n);
int right(int cc, int n);
int down(int cc, int n);
int dec_to_bin_unsigned(int dec);
void print_term(int t, int n, int bit, int group_size, char *v);
void unique_groups_of_2(int *k_map, int n, char *v);
int check_group(int *k_map, int *ncc_temp, int j, int *flag, int *ncc_final);
void arrcpy(int *a, int *b, int i);
int calculate_term_sum(int i, int *ncc, int j, int n);
void mark_elements(int *k_map, int cc, int *ncc, int j);
void unique_groups_of_4(int *k_map, int n, char *v);
void unique_groups_of_8(int *k_map, int n, char *v);
void unique_groups_of_16(int *k_map, int n, char *v);
void leftover(int *k_map, int n, char *v);
void check_group_leftover(int *k_map, int *ncc_temp, int j, int *flag, int *ncc_final, int *max);
int num_of_unmarked_elements(int *k_map, int *ncc_temp, int j);


void print_minimized_exp(char *s)
{
  int n;

  delete_spaces(s);

  n = num_of_var(s);

  printf("The minimized expression is :-\n");

  if (n == 0)
    printf("%s", s); //the expression is empty, or it is like 1, 0, (1) or (0)
  else if (n == 1)
  {
    if (s[1] == '\n' || s[2] == '\n' || s[3] == '\n' || (s[0] == '(' && s[4] == '\n'))
      printf("%s", s); //the expression is like A, A', (A) or (A')
    else if (s[1] == '+' || s[2] == '+')
      printf("1\n"); //the expression is like A+A' or A'+A
    else
      printf("0\n"); //the expression is like (A)(A'), (A')(A), (A).(A') or (A').(A)
  }
  else
    using_k_map(s);
}

void delete_spaces(char *s)
{
  int i = 0, j = 0;

  while (1)
  {
    while (s[i] == ' ')
      i++;

    s[j] = s[i];

    if (s[i] == 0)
      break;

    j++;
    i++;
  }
}

int num_of_var(char *s)
{
  int n = 0;
  char c;

  if (*s == '(')
    c = ')';
  else
    c = '+';

  while (*s != c && *s != '\n')
  {
    if (isalpha(*s))
      n++;

    s++;
  }

  return n;
}

void using_k_map(char *s)
{
  int *k_map;
  int n;
  char *v;

  k_map = form_k_map(s);

  n = num_of_var(s);

  v = form_var_char_arr(s);

  prime_implicants(k_map, n, v);
  unique_groups_of_2(k_map, n, v);
  unique_groups_of_4(k_map, n, v);
  unique_groups_of_8(k_map, n, v);
  unique_groups_of_16(k_map, n, v);
  leftover(k_map, n, v);

  printf("\b \b\b \b\b \b\n");

  free(v);
  free(k_map);
}

int* form_k_map(char *s)
{
  int *k_map;
  int n;
  char *v;
  int *cca;

  int size, i, bit, bin, j, dec;

  n = num_of_var(s);

  size = pow(2, n);

  k_map = malloc(size * sizeof(int));

  v = form_var_char_arr(s);

  cca = cca_n[n - 2];

  for (i = 0; i < size; i++)
    k_map[i] = -1;

  if (*s == '(')
  {
    bit = 0;

    s++;
  }
  else
    bit = 1;

  do
  {
    bin = 0;
    i = 0;
    while (1)
    {
      if ((bit == 1 && s[1] != '\'') || (bit == 0 && s[1] == '\''))
      {
        for (j = 0; *s != v[j]; j++);
        bin += pow(10, n - 1 - j);
      }

      i++;
      if (i == n)
        break;

      do
      {
        s++;
      } while (!isalpha(*s));
    }

    dec = bin_to_dec_unsigned(bin);
    k_map[cca[dec]] = bit;

    do
    {
      s++;
    } while (!isalpha(*s) && *s != '\n');
  } while (*s != '\n');

  free(v);

  return k_map;
}

char* form_var_char_arr(char *s)
{
  char *v;
  int n;

  int i;

  n = num_of_var(s);

  v = malloc(n * sizeof(char));

  for (i = 0; i < n; i++)
  {
    while (!isalpha(*s))
      s++;

    v[i] = *s;

    s++;
  }

  return v;
}

int bin_to_dec_unsigned(int bin)
{
  int dec = 0, p_n = 1;

  while (bin != 0)
  {
    dec += (bin % 10) * p_n;
    bin /= 10;
    p_n *= 2;
  }

  return dec;
}

void prime_implicants(int *k_map, int n, char *v)
{
  int *cca;

  int size, i, cc, cv;

  cca = cca_n[n - 2];

  size = pow(2, n);
  for (i = 0; i < size; i++)
  {
    cc = cca[i];
    cv = k_map[cc];

    if (cv == -1)
      continue;

    if (k_map[left(cc, n)] != -1)
      continue;

    if (k_map[up(cc, n)] != -1)
      continue;

    if (n != 2)
    {
      if (k_map[right(cc, n)] != -1)
        continue;
    }

    if (n == 4)
    {
      if (k_map[down(cc, n)] != -1)
        continue;
    }

    print_term(dec_to_bin_unsigned(i), n, cv, 1, v);

    k_map[cc] = 2;
  }
}

int left(int cc, int n)
{
  int lcc, c; //c :- no. of columns

  if (n == 2)
    c = 2;
  else
    c = 4;

  lcc = cc - 1;
  if (cc % c == 0)
    lcc += c;

  return lcc;
}

int up(int cc, int n)
{
  int ucc, c;

  if (n == 2)
    c = 2;
  else
    c = 4;

  ucc = cc - c;
  if (ucc < 0)
    ucc += pow(2, n);

  return ucc;
}

int right(int cc, int n)
{
  int rcc, c;

  if (n == 2)
    c = 2;
  else
    c = 4;

  rcc = cc + 1;
  if (rcc % c == 0)
    rcc -= c;

  return rcc;
}

int down(int cc, int n)
{
  int dcc, c;

  if (n == 2)
    c = 2;
  else
    c = 4;

  dcc = cc + c;
  if (dcc >= pow(2, n))
    dcc %= c;

  return dcc;
}

int dec_to_bin_unsigned(int dec)
{
  int bin = 0, p_n = 1;

  while (dec != 0)
  {
    bin += (dec % 2) * p_n;
    dec /= 2;
    p_n *= 10;
  }

  return bin;
}

void print_term(int t, int n, int bit, int group_size, char *v)
{
  int p_10, j, digit; //p_10 is power of 10

  p_10 = pow(10, n - 1);
  j = 0;
  if (bit == 1)
  {
    do
    {
      digit = t / p_10;

      if (digit == group_size)
        printf("%c", v[j]);
      else if (digit == 0)
        printf("%c'", v[j]);

      j++;
      t %= p_10;
      p_10 /= 10;
    } while (p_10 != 0);

    printf(" + ");
  }
  else
  {
    printf("\b \b\b \b\b \b"); //to clear out the trailing " . "
    printf("(");
    do
    {
      digit = t / p_10;

      if (digit == 0)
        printf("%c + ", v[j]);
      else if (digit == group_size)
        printf("%c' + ", v[j]);

      j++;
      t %= p_10;
      p_10 /= 10;
    } while (p_10 != 0);

    printf("\b \b\b \b\b \b");
    printf(")");
    printf(" . ");
  }
}

void unique_groups_of_2(int *k_map, int n, char *v)
{
  int *cca;

  int size, i, flag, cc, cv, ncc_temp[1], ncc_final[1], sum; //ncc is neighbour cc

  cca = cca_n[n - 2];

  size = pow(2, n);
  for (i = 0; i < size; i++)
  {
    flag = 0;

    cc = cca[i];
    cv = k_map[cc];

    if (cv == -1 || cv == 2)
      continue;

    ncc_temp[0] = left(cc, n);
    if (check_group(k_map, ncc_temp, 1, &flag, ncc_final) == 1)
      continue;

    ncc_temp[0] = up(cc, n);
    if (check_group(k_map, ncc_temp, 1, &flag, ncc_final) == 1)
      continue;

    if (n != 2)
    {
      ncc_temp[0] = right(cc, n);
      if (check_group(k_map, ncc_temp, 1, &flag, ncc_final) == 1)
        continue;
    }

    if (n == 4)
    {
      ncc_temp[0] = down(cc, n);
      if (check_group(k_map, ncc_temp, 1, &flag, ncc_final) == 1)
        continue;
    }

    sum = calculate_term_sum(i, ncc_final, 1, n);

    print_term(sum, n, cv, 2, v);

    mark_elements(k_map, cc, ncc_final, 1);
  }
}

int check_group(int *k_map, int *ncc_temp, int j, int *flag, int *ncc_final)
{
  int k;

  for (k = 0; k < j; k++)
  {
    if (k_map[ncc_temp[k]] == -1)
      break;
  }
  if (k == j)
  {
    if (*flag == 1)
      return 1;

    *flag = 1;
    arrcpy(ncc_final, ncc_temp, j);

    return 2;
  }

  return 0;
}

void arrcpy(int *a, int *b, int i)
{
  while (i != 0)
  {
    i--;

    a[i] = b[i];
  }
}

int calculate_term_sum(int i, int *ncc, int j, int n)
{
  int sum, k;

  int *cca;

  cca = cca_n[n - 2];

  sum = dec_to_bin_unsigned(i);
  while (j != 0)
  {
    j--;

    for (k = 0; cca[k] != ncc[j]; k++);

    sum += dec_to_bin_unsigned(k);
  }

  return sum;
}

void mark_elements(int *k_map, int cc, int *ncc, int j)
{
  k_map[cc] = 2;

  while (j != 0)
  {
    j--;

    k_map[ncc[j]] = 2;
  }
}

void unique_groups_of_4(int *k_map, int n, char *v)
{
  int i, cv, size, flag, cc, ncc_temp[3], ncc_final[3], sum;

  int *cca;

  if (n == 2)
  {
    for (i = 0; i < 4; i++)
    {
      cv = k_map[i];
      if (cv == -1 || cv == 2)
        return;
    }

    if (*k_map == 1)
      printf("1 + ");
    else
      printf("0 . ");

    for (i = 0; i < 4; i++)
      k_map[i] = 2;

    return;
  }

  cca = cca_n[n - 2];

  size = pow(2, n);
  for (i = 0; i < size; i++)
  {
    flag = 0;

    cc = cca[i];
    cv = k_map[cc];

    if (cv == -1 || cv == 2)
      continue;

    ncc_temp[0] = left(cc, n); //check horizontal
    ncc_temp[1] = right(cc, n);
    ncc_temp[2] = right(ncc_temp[1], n);
    if (check_group(k_map, ncc_temp, 3, &flag, ncc_final) == 1)
      continue;

    ncc_temp[0] = left(cc, n); //check left
    ncc_temp[1] = up(ncc_temp[0], n);
    ncc_temp[2] = up(cc, n);
    if (check_group(k_map, ncc_temp, 3, &flag, ncc_final) == 1)
      continue;

    ncc_temp[0] = up(cc, n); //check up
    ncc_temp[1] = right(ncc_temp[0], n);
    ncc_temp[2] = right(cc, n);
    if (check_group(k_map, ncc_temp, 3, &flag, ncc_final) == 1)
      continue;

    if (n == 4)
    {
      ncc_temp[0] = up(cc, n); //check vertical
      ncc_temp[1] = down(cc, n);
      ncc_temp[2] = down(ncc_temp[1], n);
      if (check_group(k_map, ncc_temp, 3, &flag, ncc_final) == 1)
        continue;

      ncc_temp[0] = right(cc, n); //check right
      ncc_temp[1] = down(ncc_temp[0], n);
      ncc_temp[2] = down(cc, n);
      if (check_group(k_map, ncc_temp, 3, &flag, ncc_final) == 1)
        continue;

      ncc_temp[0] = down(cc, n); //check down
      ncc_temp[1] = left(ncc_temp[0], n);
      ncc_temp[2] = left(cc, n);
      if (check_group(k_map, ncc_temp, 3, &flag, ncc_final) == 1)
        continue;
    }

    if (flag == 0)
      continue;

    sum = calculate_term_sum(i, ncc_final, 3, n);

    print_term(sum, n, cv, 4, v);

    mark_elements(k_map, cc, ncc_final, 3);
  }
}

void unique_groups_of_8(int *k_map, int n, char *v)
{
  int i, cv, size, flag, cc, ncc_temp[7], ncc_final[7], sum;

  int *cca;

  if (n == 2)
    return;
  else if (n == 3)
  {
    for (i = 0; i < 8; i++)
    {
      cv = k_map[i];
      if (cv == -1 || cv == 2)
        return;
    }

    if (*k_map == 1)
      printf("1 + ");
    else
      printf("0 . ");

    for (i = 0; i < 8; i++)
      k_map[i] = 2;

    return;
  }

  cca = cca_n[n - 2];

  size = pow(2, n);
  for (i = 0; i < size; i++)
  {
    flag = 0;

    cc = cca[i];
    cv = k_map[cc];

    if (cv == -1 || cv == 2)
      continue;

    ncc_temp[0] = left(cc, n); //check left
    ncc_temp[1] = up(ncc_temp[0], n);
    ncc_temp[2] = up(ncc_temp[1], n);
    ncc_temp[3] = up(ncc_temp[2], n);
    ncc_temp[4] = right(ncc_temp[3], n);
    ncc_temp[5] = down(ncc_temp[4], n);
    ncc_temp[6] = up(cc, n);
    if (check_group(k_map, ncc_temp, 7, &flag, ncc_final) == 1)
      continue;

    ncc_temp[0] = up(cc, n); //check up
    ncc_temp[1] = right(ncc_temp[0], n);
    ncc_temp[2] = right(ncc_temp[1], n);
    ncc_temp[3] = right(ncc_temp[2], n);
    ncc_temp[4] = down(ncc_temp[3], n);
    ncc_temp[5] = left(ncc_temp[4], n);
    ncc_temp[6] = right(cc, n);
    if (check_group(k_map, ncc_temp, 7, &flag, ncc_final) == 1)
      continue;

    ncc_temp[0] = right(cc, n); //check right
    ncc_temp[1] = down(ncc_temp[0], n);
    ncc_temp[2] = down(ncc_temp[1], n);
    ncc_temp[3] = down(ncc_temp[2], n);
    ncc_temp[4] = left(ncc_temp[3], n);
    ncc_temp[5] = up(ncc_temp[4], n);
    ncc_temp[6] = down(cc, n);
    if (check_group(k_map, ncc_temp, 7, &flag, ncc_final) == 1)
      continue;

    ncc_temp[0] = down(cc, n); //check down
    ncc_temp[1] = left(ncc_temp[0], n);
    ncc_temp[2] = left(ncc_temp[1], n);
    ncc_temp[3] = left(ncc_temp[2], n);
    ncc_temp[4] = up(ncc_temp[3], n);
    ncc_temp[5] = right(ncc_temp[4], n);
    ncc_temp[6] = left(cc, n);
    if (check_group(k_map, ncc_temp, 7, &flag, ncc_final) == 1)
      continue;

    if (flag == 0)
      continue;

    sum = calculate_term_sum(i, ncc_final, 7, n);

    print_term(sum, n, cv, 8, v);

    mark_elements(k_map, cc, ncc_final, 7);
  }
}

void unique_groups_of_16(int *k_map, int n, char *v)
{
  int i, cv;

  if (n <= 3)
    return;

  for (i = 0; i < 16; i++)
  {
    cv = k_map[i];
    if (cv == -1 || cv == 2)
      return;
  }

  if (*k_map == 1)
    printf("1 + ");
  else
    printf("0 . ");

  for (i = 0; i < 16; i++)
    k_map[i] = 2;

  return;
}

void leftover(int *k_map, int n, char *v)
{
  int i, cv, size, flag, cc, ncc_temp[7], ncc_final[7], sum, max, j;

  int *cca;

  cca = cca_n[n - 2];

  size = pow(2, n);
  for (i = 0; i < size; i++)
  {
    flag = 0;
    max = -1;

    cc = cca[i];
    cv = k_map[cc];

    if (cv == -1 || cv == 2)
      continue;

    if (n == 4)
    {
      j = 7;

      ncc_temp[0] = left(cc, n);
      ncc_temp[1] = up(ncc_temp[0], n);
      ncc_temp[2] = up(ncc_temp[1], n);
      ncc_temp[3] = up(ncc_temp[2], n);
      ncc_temp[4] = right(ncc_temp[3], n);
      ncc_temp[5] = down(ncc_temp[4], n);
      ncc_temp[6] = up(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      ncc_temp[0] = up(cc, n);
      ncc_temp[1] = right(ncc_temp[0], n);
      ncc_temp[2] = right(ncc_temp[1], n);
      ncc_temp[3] = right(ncc_temp[2], n);
      ncc_temp[4] = down(ncc_temp[3], n);
      ncc_temp[5] = left(ncc_temp[4], n);
      ncc_temp[6] = right(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      ncc_temp[0] = right(cc, n);
      ncc_temp[1] = down(ncc_temp[0], n);
      ncc_temp[2] = down(ncc_temp[1], n);
      ncc_temp[3] = down(ncc_temp[2], n);
      ncc_temp[4] = left(ncc_temp[3], n);
      ncc_temp[5] = up(ncc_temp[4], n);
      ncc_temp[6] = down(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      ncc_temp[0] = down(cc, n);
      ncc_temp[1] = left(ncc_temp[0], n);
      ncc_temp[2] = left(ncc_temp[1], n);
      ncc_temp[3] = left(ncc_temp[2], n);
      ncc_temp[4] = up(ncc_temp[3], n);
      ncc_temp[5] = right(ncc_temp[4], n);
      ncc_temp[6] = left(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);
    }

    if (flag == 0 && n != 2)
    {
      j = 3;

      ncc_temp[0] = left(cc, n);
      ncc_temp[1] = right(cc, n);
      ncc_temp[2] = right(ncc_temp[1], n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      ncc_temp[0] = left(cc, n);
      ncc_temp[1] = up(ncc_temp[0], n);
      ncc_temp[2] = up(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      ncc_temp[0] = up(cc, n);
      ncc_temp[1] = right(ncc_temp[0], n);
      ncc_temp[2] = right(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      if (n == 4)
      {
        ncc_temp[0] = up(cc, n);
        ncc_temp[1] = down(cc, n);
        ncc_temp[2] = down(ncc_temp[1], n);
        check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

        ncc_temp[0] = right(cc, n);
        ncc_temp[1] = down(ncc_temp[0], n);
        ncc_temp[2] = down(cc, n);
        check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

        ncc_temp[0] = down(cc, n);
        ncc_temp[1] = left(ncc_temp[0], n);
        ncc_temp[2] = left(cc, n);
        check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);
      }
    }

    if (flag == 0)
    {
      j = 1;

      ncc_temp[0] = left(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      ncc_temp[0] = up(cc, n);
      check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);

      if (n != 2)
      {
        ncc_temp[0] = right(cc, n);
        check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);
      }

      if (n == 4)
      {
        ncc_temp[0] = down(cc, n);
        check_group_leftover(k_map, ncc_temp, j, &flag, ncc_final, &max);
      }
    }

    sum = calculate_term_sum(i, ncc_final, j, n);

    print_term(sum, n, cv, j + 1, v);

    mark_elements(k_map, cc, ncc_final, j);
  }
}

void check_group_leftover(int *k_map, int *ncc_temp, int j, int *flag, int *ncc_final, int *max)
{
  int current;

  if (check_group(k_map, ncc_temp, j, flag, ncc_final) != 0)
  {
    current = num_of_unmarked_elements(k_map, ncc_temp, j);

    if (current > *max)
    {
      if (*max != -1)
        arrcpy(ncc_final, ncc_temp, j);

      *max = current;
    }
  }
}

int num_of_unmarked_elements(int *k_map, int *ncc_temp, int j)
{
  int k, num = 0;

  for (k = 0; k < j; k++)
  {
    if (k_map[ncc_temp[k]] != 2)
      num++;
  }

  return num;
}


int main()
{
  char *s = 0;
  unsigned long int t = 0;

  printf("Enter the SSOP/SPOS expression :-\n");
  getline(&s, &t, stdin);

  print_minimized_exp(s);

  free(s);

  return 0;
}
