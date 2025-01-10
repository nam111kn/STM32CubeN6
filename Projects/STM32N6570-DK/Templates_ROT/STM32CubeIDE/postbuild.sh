#!/bin/bash -

# arg1 is the binary type (1 nonsecure, 2 secure)
signing=$1

# arg2 is the config type (Debug, Release)
config=$2

# Getting provisioning path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../ROT_Provisioning"
provisioningdir=$(pwd)
cd "$project_dir"
source "$provisioningdir/env.sh" $provisioningdir

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

# Environment variable for log file
current_log_file=postbuild.log

s_app_init_xml=$provisioningdir/OEMuROT/Images/OEMuROT_S_Code_Init_Image.xml
ns_app_init_xml=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Code_Init_Image.xml

s_app_xml=$provisioningdir/OEMuROT/Images/OEMuROT_S_Code_Image.xml
ns_app_xml=$provisioningdir/OEMuROT/Images/OEMuROT_NS_Code_Image.xml

bin_dest_dir=../../../$oemurot_appli_path_project/Binary

s_app_bin_cube=$project_dir/AppliSecure/$config/Template_ROT_AppliSecure.bin
s_app_bin=$bin_dest_dir/rot_tz_s_app.bin
s_app_init_sign_bin=$bin_dest_dir/rot_tz_s_app_init_enc_sign.bin
s_app_sign_bin=$bin_dest_dir/rot_tz_s_app_enc_sign.bin

ns_app_bin_cube=$project_dir/AppliNonSecure/$config/Template_ROT_AppliNonSecure.bin
ns_app_bin=$bin_dest_dir/rot_tz_ns_app.bin
ns_app_init_sign_bin=$bin_dest_dir/rot_tz_ns_app_init_enc_sign.bin
ns_app_sign_bin=$bin_dest_dir/rot_tz_ns_app_enc_sign.bin

applicfg=$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$applicfg" ]; then
  #line for window executable
  echo "AppliCfg with windows executable"
  python=
else
  #line for python
  echo "AppliCfg with python script"
  applicfg=$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py
  #determine/check python version command
  python="python3 "
fi

if  [ $signing == "secure" ]; then
  echo "Copy generated binary to binaries folder" > "$current_log_file"
  cp "$s_app_bin_cube" ../Binary/rot_tz_s_app.bin >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

  echo "Creating secure signed image"  >> "$current_log_file"
	# update xml file : input file
	"$python$applicfg" xmlval -v "$s_app_bin" --string -n "Firmware binary input file" "$s_app_init_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi
	
	# update xml file : input file
	"$python$applicfg" xmlval -v "$s_app_bin" --string -n "Firmware binary input file" "$s_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

	# update xml file : output file
	"$python$applicfg" xmlval -v "$s_app_init_sign_bin" --string -n "Image output file" "$s_app_init_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

	# update xml file : output file
	"$python$applicfg" xmlval -v "$s_app_sign_bin" --string -n "Image output file" "$s_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

	# Creating signed/encrypted image
	"$stm32tpccli" -pb "$s_app_init_xml" >> "$current_log_file" 2>&1
	if [ $? != 0 ]; then error; fi
	"$stm32tpccli" -pb "$s_app_xml" >> "$current_log_file" 2>&1
	if [ $? != 0 ]; then error; fi
fi

if  [ $signing == "nonsecure" ]; then
  echo "Copy generated binary to binaries folder" > $current_log_file
  cp "$ns_app_bin_cube" ../Binary/rot_tz_ns_app.bin >> $current_log_file 2>&1
  if [ $? != 0 ]; then error; fi

  echo "Creating non secure signed image"  >> "$current_log_file"
	# Update xml file : input file
	"$python$applicfg" xmlval -v "$ns_app_bin" --string -n "Firmware binary input file" "$ns_app_init_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

	# update xml file : input file
	"$python$applicfg" xmlval -v "$ns_app_bin" --string -n "Firmware binary input file" "$ns_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi

	# update xml file : output file
	"$python$applicfg" xmlval -v "$ns_app_init_sign_bin" --string -n "Image output file" "$ns_app_init_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi
	
	# update xml file : output file
	"$python$applicfg" xmlval -v "$ns_app_sign_bin" --string -n "Image output file" "$ns_app_xml" --vb >> "$current_log_file" 2>&1
  if [ $? != 0 ]; then error; fi
	# Creating signed/encrypted image
	"$stm32tpccli" -pb "$ns_app_init_xml" >> "$current_log_file" 2>&1
	if [ $? != 0 ]; then error; fi
	"$stm32tpccli" -pb "$ns_app_xml" >> "$current_log_file" 2>&1
	if [ $? != 0 ]; then error; fi
fi

exit 0


