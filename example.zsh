#prepare key file
cat JIC_GBS0865_v2.txt | tr '\r' '\n' | column -t | awk '{print $3"\t"$8}' | tail -n +2 > 160406_keyfile.txt

#create output directory
mkdir GBS0865xKCIAJohnInnesGRU

#run splitgbs
./splitgbs GBS0865xKCIAJohnInnesGRU 160406_keyfile.txt GBS0865xKCIAJohnInnesGRU_HL73LADXX_s_2_fastq.txt.gz 2> 160406_keyfile_splitgbs.err
