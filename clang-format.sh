find . -regex '.*\.\(cc\|cpp\|hh\|h\)' -exec clang-format -i {} \;
