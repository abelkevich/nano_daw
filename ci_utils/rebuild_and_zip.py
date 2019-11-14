import os
import zipfile
import argparse

def pack(from_path, zip_dir, zip_name):
	zipf = zipfile.ZipFile(os.path.join(zip_dir, zip_name + '.zip'), 'w', zipfile.ZIP_DEFLATED)
	
	for root, dirs, files in os.walk(from_path):
		for file in files:
			file_path = os.path.join(root, file)
			zip_path = '%s\\%s' % (zip_name, file_path.split(from_path)[1])
			zipf.write(file_path, zip_path)
	
	zipf.close()
	
def rebuild(sln_path, platform, config):
	msbuild_cmd = 'devenv.com %s /rebuild "%s|%s"' % (sln_path, config, platform)
	os.system(msbuild_cmd)
	
def main():
	parser = argparse.ArgumentParser(description='Zip nano_daw')
	parser.add_argument('platform', type=str, help='VS Platform: x86 or x64')
	parser.add_argument('config', type=str, help='VS Config: Debug or Release')
	parser.add_argument('zip_version', type=str, help='Zip build version')
	parser.add_argument('zip_dir', type=str, help='Zip output directory')
	
	args = parser.parse_args()

	rebuild('ide\\vs2019\\nano_daw.sln', args.platform, args.config)

	platform_path = "Win32" if (args.platform == "x86") else "x64"
	from_path = 'bin\\%s\\%s' % (platform_path, args.config)
	zip_name = "nano_daw_%s_%s_%s" % (args.zip_version, args.platform, args.config)
	pack(from_path, args.zip_dir, zip_name)
	
if __name__ == '__main__':
	main()
