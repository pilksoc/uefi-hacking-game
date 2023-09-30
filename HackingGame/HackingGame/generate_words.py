with open("./english3.txt", "r") as file:
    words = list(
        map(lambda x: x[:-1],
            filter(lambda x: len(x) == 6 and "'" not in x, [line for line in file])))
    with open("./words.c", "w") as output:
        print("""/*
AUTO GENERATED FILE - DO NOT EDIT BY HAND

This file was generated with ./generate_words.py.
*/""",
              file=output)
        print('#include "./words.h"', file=output)

        print("""size_t hg_word_count()
{
""", file=output)
        print(f"    return {len(words)};", file=output)
        print("}", file=output)

        print("""int hg_word_at(size_t index, hg_word_t ret)
{
    switch(index) {""",
              file=output)
        for i in range(len(words)):
            print(f'case {i}:', file=output)
            j = 0
            for char in list(words[i]):
                print(f"    ret[{j}] = '{char}';", file=output)
                j += 1
            print('    return 1;', file=output)

        print("""default: return 0;
    }
}""", file=output)
