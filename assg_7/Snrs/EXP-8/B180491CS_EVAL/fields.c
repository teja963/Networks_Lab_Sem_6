#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int starts_with(char *full, char *sub)
{
    int m = strlen(full);
    int n = strlen(sub);

    if (n > m)
        return 0;

    for (int i = 0; i < n; i++)
        if (full[i] != sub[i])
            return 0;

    return n;
}

int has_char(char *str, char c)
{
    int n = strlen(str);
    for (int i = 0; i < n; i++)
        if (str[i] == c)
            return i;
    return -1;
}

void get_line(char *data, int *index, char *out)
{
    int n = strlen(data);
    int i = 0;

    while (data[*index] != '\n' && *index < n)
    {
        out[i] = data[*index];
        *index += 1;
        i++;
    }

    out[i] = '\0';

    if (data[*index] == '\n')
        *index += 1;
}

void get_field(char *data, char *field, char *out)
{
    char buffer[100];
    int index = 0, n = 0;
    int size = strlen(data);
    while (index < size)
    {
        get_line(data, &index, buffer);
        if (has_char(buffer, ':') == -1)
        {
            *out = 0;
            if (strcmp(field, "Body") == 0)
            {
                sprintf(out, "%s\n%s", buffer, data + index);
            }
            return;
        }

        if (n = starts_with(buffer, field))
        {
            // +2 to skip the ": "
            strcpy(out, buffer + n + 2);
            return;
        }
    }
}

int verify_email(char *email)
{
    int n = strlen(email);
    int x = 0;
    int y = 0;
    int flag = 0;
    for (int i = 0; i < n; i++)
    {
        if (email[i] == '@')
        {
            flag = 1;
            continue;
        }

        if (flag == 0)
            x++;
        else
            y++;

        if (y >= 1)
            break;
    }

    return x > 0 && y > 0;
}