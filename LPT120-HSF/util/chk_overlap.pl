#! /usr/bin/perl -w

my $fn_log = "dump.log";
my $bss_start = 0;
my $BSS_ALLOW_MAX = 0x2000000 + (132 * 1024); #old design may only allow bss_start from 132KB#

open(FIN,"$fn_log") || die "cannot open $fn_log";

while(<FIN>)
{
	chomp;
	if(/\.bss\s+(\S+)\s+(\S+)/){
		$bss_start = hex($2); 
		#printf "$_ bss_start=0x%x\n",$bss_start;
		if($bss_start > $BSS_ALLOW_MAX)
		{
			printf "\n\n";
			printf "bss_start=0x%x\n",$bss_start;
			printf "BSS_ALLOW_MAX=0x%x\n",$BSS_ALLOW_MAX;  

			printf "###############################################\n";
			printf "#    ERROR: OS_DATA may corrupt boot-sector   #\n";
			printf "###############################################\n";
			printf "\n\n";
			exit(-1);
		}
	}

}

close(FIN);

