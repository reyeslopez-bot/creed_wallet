# build_operations.py
import logging
import os
import subprocess

def run_command(command: list[str], cwd: str | None = None) -> subprocess.CompletedProcess:
    try:
        cwd_display = cwd or os.getcwd()
        logging.info(f"Running command: {' '.join(command)} in {cwd_display}")
        result = subprocess.run(command, cwd=cwd, check=True, text=True, capture_output=True)
        log_command_output(result)
    except subprocess.CalledProcessError as e:
        logging.error(f"Command '{' '.join(command)}' failed with return code {e.returncode}")
        logging.error(f"Command output:\n{e.stdout}")
        logging.error(f"Command error output:\n{e.stderr}")
        raise e
    except Exception as e:
        logging.exception(f"Unexpected error while running command: {' '.join(command)}")
        raise e

def log_command_output(result):
    logging.info(f"Command output:\n{result.stdout}")
    if result.stderr:
        logging.warning(f"Command stderr:\n{result.stderr}")

def clean_build_directory(build_dir='build'):
    if os.path.exists(build_dir):
        logging.info(f"Cleaning previous build directory: {build_dir}")
        run_command(['rm', '-rf', build_dir])

def create_build_directory(build_dir='build'):
    try:
        logging.info(f"Creating new build directory: {build_dir}")
        os.mkdir(build_dir)
    except OSError as e:
        logging.error(f"Failed to create build directory {build_dir}: {e}")
        raise e

def check_makefile(build_dir='build'):
    makefile_path = os.path.join(build_dir, 'Makefile')
    if not os.path.exists(makefile_path):
        logging.error(f"Makefile not found in {build_dir}. Build cannot proceed.")
        raise FileNotFoundError(f"Makefile not found in {build_dir}")

def build_project(build_dir='build'):
    logging.info("Starting the build process...")
    try:
        clean_build_directory(build_dir)
        create_build_directory(build_dir)

        logging.info("Running CMake...")
        run_command(['cmake', f'-DCMAKE_PREFIX_PATH={os.popen("brew --prefix pybind11").read().strip()}', '-S', '.', '-B', build_dir], cwd='.')

        check_makefile(build_dir)
        logging.info("Running Make...")
        run_command(['make'], cwd=build_dir)

        logging.info("Build completed successfully.")
        return True
    except Exception as e:
        logging.error(f"Build process failed: {e}")
        print("Build process failed. Please check the logs for details.")
        return False
