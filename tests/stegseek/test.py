import os
from glob import glob
from subprocess import Popen, PIPE


data_dir = "data"
file_samples_dir = "../steghide/data"
tmp_dir = "test_tmp"
stegseek = "../../build/src/stegseek"
extensions = ["*.jpg", "*.jpeg", "*.bmp", "*.au", "*.wav"]

def setup():
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
    else:
        files = glob(os.path.join(tmp_dir, '*'))
        for f in files:
            os.remove(f)

def teardown():
    files = glob(os.path.join(tmp_dir, '*'))
    for f in files:
        os.remove(f)

def basic_crack():
    wordlist_path = os.path.join(tmp_dir, "wordlist.txt")
    steg_path = os.path.join(data_dir, "sun.jpg")

    # Create a large wordlist, without the correct password present.
    with open(wordlist_path, 'w') as wordlist:
        wordlist.writelines([f"Blah_{i}\n" for i in range(9999)])

    p = Popen([stegseek, steg_path, wordlist_path, "-"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    p.communicate()
    assert p.returncode == 1, f"Return code was {p.returncode}, expected 1."

    # Add the correct password, along with some more bogus
    with open(wordlist_path, 'w+') as wordlist:
        wordlist.writelines(f"stegseek\n")
        wordlist.writelines([f"Blah_{i}\n" for i in range(9999)])

    p = Popen([stegseek, steg_path, wordlist_path, "-"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    p.communicate()

    print("Basic wordlist cracking.")
    assert p.returncode == 0, f"Return code was {p.returncode}, expected 0."


# Make sure the way the list is divided over threads doesn't break anything
def tiny_list():
    wordlist_path = os.path.join(tmp_dir, "wordlist.txt")
    steg_path = os.path.join(data_dir, "sun.jpg")

    # Create a tiny wordlist, with only the correct password
    with open(wordlist_path, 'w') as wordlist:
        wordlist.writelines(["stegseek\n"])

    p = Popen([stegseek, steg_path, wordlist_path, "-t", "20", "-s", "-"], 
            stdin=PIPE, stdout=PIPE, stderr=PIPE)
    p.communicate()

    print("testing cracker scenario where num threads > wordlist size.")
    assert p.returncode == 0, f"Return code was {p.returncode}, expected 0."


# This test triggers a specicial case in 'verifyMagic'
def rng_collision():
    wordlist_path = os.path.join(tmp_dir, "wordlist.txt")
    steg_path = os.path.join(data_dir, "rngcollision.jpg")

    # Create a wordlist
    with open(wordlist_path, 'w') as wordlist:
        wordlist.writelines([f"Blah_{i}\n" for i in range(100)])
        wordlist.writelines(["iluv1josh\n"])
        wordlist.writelines([f"Blah_{i}\n" for i in range(100)])

    p = Popen([stegseek, steg_path, wordlist_path, "-"], 
            stdin=PIPE, stdout=PIPE, stderr=PIPE)
    p.communicate()
    print("testing cracker scenario with an RNG collision in the selector.")
    assert p.returncode == 0, f"Return code was {p.returncode}, expected 0."

# test different file types
def file_types_embed():
    password = "BeepBoop123"
    # Create a file to embed
    embed_path = os.path.join(tmp_dir, "embed_file.txt")
    with open(embed_path, 'w') as embed_file:
        embed_file.writelines(["This is a test file\n"])
    # First, embed some data in each file type
    files_by_ext = {ext: glob(os.path.join(file_samples_dir, ext)) for ext in extensions}

    for (ext, files) in files_by_ext.items():
        print(f"Embedding in '{ext}'")
        for source_file in files:
            steg_path = os.path.join(tmp_dir, os.path.basename(source_file))
            p = Popen([stegseek, "--embed", embed_path, source_file, steg_path, "-p", password], 
                    stdin=PIPE, stdout=PIPE, stderr=PIPE)
            p.communicate()
            assert p.returncode == 0, f"[{source_file}] Return code was {p.returncode}, expected 0."

def file_types_extract():
    password = "BeepBoop123"

    # First, embed some data in each file type
    files_by_ext = {ext: glob(os.path.join(tmp_dir, ext)) for ext in extensions}

    for (ext, files) in files_by_ext.items():
        print(f"Extracting from '{ext}'")
        for steg_file in files:
            p = Popen([stegseek, "--extract", steg_file, "-", "-p", password], 
                    stdin=PIPE, stdout=PIPE, stderr=PIPE)
            stdout, stderr = p.communicate()
            assert p.returncode == 0, f"[{steg_file}] Return code was {p.returncode}, expected 0."
            assert b"This is a test file" in stdout, f"[{steg_file}] Expected file content was not extracted."


# Minimal wordlist, tests custom verifyMagic on all file types
def file_types_crack():
    password = "BeepBoop123"

    # First, get a list of all file types
    files_by_ext = {ext: glob(os.path.join(tmp_dir, ext)) for ext in extensions}

    wordlist_path = os.path.join(tmp_dir, "wordlist.txt")

    # Create a tiny wordlist, with only the correct password
    with open(wordlist_path, 'w') as wordlist:
        wordlist.writelines([f"{password}\n"])

    for (ext, files) in files_by_ext.items():
        print(f"Cracking a '{ext}' file.")
        for steg_file in files:
            p = Popen([stegseek, steg_file, wordlist_path, "-"], 
                    stdin=PIPE, stdout=PIPE, stderr=PIPE)
            stdout, stderr = p.communicate()
            assert p.returncode == 0, f"[{steg_file}] Return code was {p.returncode}, expected 0."
            assert b"This is a test file" in stdout, f"[{steg_file}] Failed to crack stego file."

def basic_seed_cracking():
    password = "moana5" # Seed = 657
    embed_path = os.path.join(tmp_dir, "embed_file.txt")
    with open(embed_path, 'w') as embed_file:
        embed_file.writelines(["This is a test file\n"])
    source_file = os.path.join(file_samples_dir, "std.jpg")
    steg_path = os.path.join(tmp_dir, "steg-seed-01.jpg")
    # First, embed some unencrypted data in a file
    p = Popen([stegseek, "--embed", embed_path, source_file, steg_path, "-p", password,
        '-e', 'none'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    p.communicate()
    assert p.returncode == 0, f"[{source_file}] Return code was {p.returncode}, expected 0."

    # Next, try to extract the files contents with seed cracking. The password is picked s.t. 
    # this should finish almost instantly which makes it more practical to run this test in CI,
    p = Popen([stegseek, "--seed", steg_path, "-"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate(timeout=5)
    print("Testing basic seed cracking.")
    assert p.returncode == 0, f"[{source_file}] Return code was {p.returncode}, expected 0."
    assert b"This is a test file" in stdout, f"[{source_file}] Failed to extract plaintext."
    assert b"00000291" in stderr, f"[{source_file}] Expected seed not found."

def file_types_seed_crack():
    password = "apple87" # Seed = 3610 = 0xe1a

    # Create a file to embed
    embed_path = os.path.join(tmp_dir, "embed_file.txt")
    with open(embed_path, 'w') as embed_file:
        embed_file.writelines(["This is a test file\n"])
    # Embed some data in each file type
    files_by_ext = {ext: glob(os.path.join(file_samples_dir, ext)) for ext in extensions}

    for (ext, files) in files_by_ext.items():
        for source_file in files:
            steg_path = os.path.join(tmp_dir, os.path.basename(source_file))
            p = Popen([stegseek, "--embed", embed_path, source_file, steg_path, "-p", password,
                '-e', 'none', '-f'], stdin=PIPE, stdout=PIPE, stderr=PIPE)
            p.communicate()
            assert p.returncode == 0, f"[{source_file}] Return code was {p.returncode}, expected 0."

            # Now, attempt to crack the file
            p = Popen([stegseek, "--seed", steg_path, "-"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
            stdout, stderr = p.communicate(timeout=5)
            print(f"Cracking the seed of a \"{ext}\" file.")
            assert p.returncode == 0, f"[{source_file}] Return code was {p.returncode}, expected 0."
            assert b"This is a test file" in stdout, f"[{source_file}] Failed to extract plaintext."
            assert b"00000e1a" in stderr, f"[{source_file}] Expected seed not found."


if __name__ == "__main__":
    setup()

    # File type tests
    file_types_embed()
    file_types_extract()

    # Cracking tests
    basic_crack()
    tiny_list()
    rng_collision()
    file_types_crack()

    # Seed cracking
    basic_seed_cracking()
    file_types_seed_crack()

    teardown()
