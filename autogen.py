import operator
import re
import os
from glob import glob

# Functions for reading and writing files
def read_from_file(file_name):
    """
    Function for reading text files
    """

    f = open(file_name, 'r')
    text = f.read()
    f.close()
    return text


def write_to_file(text, file_name, writeoption='w'):
    """
    Function writing text files.
    """
    
    f = open(file_name, writeoption)
    f.write(text)
    f.close()

def get_files(folders, filepatterns=['\w+\.h$', '\w+\.cpp$'], exclude_folders=[], exclude=[]):
    """
    Function for getting the name of files matching a given patter in
    a list of folders.
    """

    all_files = reduce(operator.add, map(lambda folder: glob(folder+"*"), folders))
    files = []

    for filename in all_files:
        fn = os.path.split(filename)[1]

        match = False
        for regexp in filepatterns:
            match = match or re.match(regexp, fn)

        if match and fn not in exclude:
            files.append(filename)

    return files


def get_files_recursively(folder, filepatterns=['\w+\.h$', '\w+\.cpp$'], exclude=[], exclude_subfolders=None):
    """
    Function for getting the name of files matching a given patter in a folder and it's subfolders.
    """

    files = []

    for root, dirnames, filenames in os.walk(folder):

        if exclude_subfolders==None or exclude_subfolders not in root:
            for filename in filenames:
                fn = os.path.split(filename)[1]

                match = False
                for regexp in filepatterns:
                    match = match or re.match(regexp, filename)

                if match and filename not in exclude:
                    files.append(root+"/"+filename)
                
    return files




def set_varaible_in_makefile(makefilename, variable, value):
    """
    Function for editing a Makefile.am.
    """

    txt = read_from_file(makefilename)

    regexp = re.compile("^(?P<def>[ \t]*%s\s[ \t]*=).*" % variable, re.MULTILINE)


    (newtxt, count) = regexp.subn("\g<def> %s" % value, txt)

    if count != 1:
        print "WARNING: found %d occurrences of the variable '%s' in '%s'" % (count, variable, makefilename)

    write_to_file(newtxt, makefilename)

def set_function_arguments_in_cmakelists(cmakelistsfilename, function, first_argument, remaining_arguments):
    """
     Function for editing a CMakeLists.txt.
    """

    txt = read_from_file(cmakelistsfilename)

    regexp = re.compile("^(?P<def>[ \t]*%s\(%s).*\)" % (function,first_argument), re.MULTILINE | re.DOTALL)

    (newtxt, count) = regexp.subn("\g<def> %s\n)" % remaining_arguments, txt)

    if count != 1:
        print "WARNING: found %d occurrences of the variable '%s' in '%s'" % (count, variable, makefilename)

    write_to_file(newtxt, cmakelistsfilename)


if __name__ == "__main__":
    import os

    # Set extra files for distribution
    text_files = get_files_recursively(".", ['\w+\.txt$'], exclude_subfolders=".svn")
    script_files = get_files_recursively("scripts", ['\w+\.py$'], exclude_subfolders=".svn")
    eigen_files = get_files_recursively("external", ['[^.]+$','\w+\.cpp$', '\w+\.h$'], exclude_subfolders=".svn")

    set_varaible_in_makefile("Makefile.am", "EXTRA_DIST", " ".join(text_files+script_files+eigen_files))


    # Go to the Muninn directory
    os.chdir("muninn")

    # Set the files for Muninn library in Automake
    muninn_folders = ["",
                      "Binners/",
                      "Exceptions/",
                      "Factories/",
                      "Histories/",
                      "MLE/",
                      "MLE/utils/",
                      "tools/",
                      "tools/phaistos/",
                      "UpdateSchemes/",
                      "utils/",
                      "utils/nonlinear/",
                      "utils/nonlinear/nr/",
                      "utils/nonlinear/newton/",
                      "utils/polation/",
                      "WeightSchemes/",
                      ]

    muninn_files = get_files(muninn_folders, ['\w+\.cpp$', '\w+\.f$'])
    set_varaible_in_makefile("Makefile.am", "libmuninn_la_SOURCES", " ".join(muninn_files))

    # Set the files for Muninn library in Cmake
    set_function_arguments_in_cmakelists("CMakeLists.txt", "add_library", "muninn", "\n  " + "\n  ".join(muninn_files))

    # Set all the headers for inclusion
    header_files = get_files(muninn_folders, ['\w+\.h$'])
    header_files = filter(lambda filename: filename not in ['utils/TArrayLA.h'], header_files)
    set_varaible_in_makefile("Makefile.am", "nobase_pkginclude_HEADERS", " ".join(header_files))
