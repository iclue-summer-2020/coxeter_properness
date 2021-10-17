#!/usr/bin/env python3
import argparse
import datetime as dt
import logging
import pathlib
import os
import subprocess
import tempfile
from typing import IO, Any, Final, List, Optional, Text, Union
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass

import py7zr
from halo import Halo

CMAKE_BUILD_DIR: Final = "cmake-build-debug"
RUN: Final = f"./{CMAKE_BUILD_DIR}/apps/{CMAKE_BUILD_DIR}/run"

logger = logging.getLogger(__name__)
FORMAT = "[%(filename)s:%(lineno)s - %(funcName)20s()] %(message)s"
logging.basicConfig(format=FORMAT)
logger.setLevel(logging.DEBUG)

@dataclass(frozen=True)
class ProcessInfo:
  coxeter_type: str
  rank: int
  in_path: str
  out_path: str
  archive: str

def process(info: ProcessInfo) -> None:
  with tempfile.TemporaryDirectory() as temp_dirname:
    with Halo(text=f"extracting {info.in_path} from {info.archive}"):
      with py7zr.SevenZipFile(info.in_path, mode='r') as z:
        z.extract(path=temp_dirname, targets=[info.in_path])
    
    # temp_path = pathlib.Path(f"{temp_dirname}/{file_info.filename}")

    with Halo(text=f"processing {info.in_path}") as spinner:
      # out_path = pathlib.Path(f"{out_dir.resolve()}/{file_info.filename}")
      process = subprocess.run([
        RUN,
        "--type", info.coxeter_type,
        "--rank", str(info.rank),
        "--in-file", info.in_path,
        "--out-file", info.out_path,
      ])

    if process.returncode != 0:
      logger.error(f"something went wrong:\n\t{process.args=}\n\t{process.stdout=!r}\n\t{process.stderr=!r}")
      spinner.fail("something went wrong")

    spinner.text = f"removing file {temp_dirname}"
  spinner.succeed("removed file")


def main(args: argparse.Namespace) -> None:
  in_file: pathlib.Path = args.in_file
  out_dir: pathlib.Path = args.out_dir
  start_time = dt.datetime.now()

  with py7zr.SevenZipFile(in_file, mode='r') as z:
    infos = z.list()
  
  file_infos = sorted([
      ProcessInfo(
        coxeter_type=args.type,
        rank=args.n,
        in_path=info.filename,
        out_path=f"{out_dir.resolve().name}/{info.filename}",
        archive=in_file.resolve().name,
      )
      for info in infos
    ], key=lambda info: int(info.in_path[3:])
  )
    
  with Halo(text="processing {args.type}{args.n}") as spinner:
    with ThreadPoolExecutor() as pool:
      pool.map(process, file_infos)

  end_time = dt.datetime.now()
  elapsed_time = end_time - start_time
  spinner.succeed(f"finished in {elapsed_time.seconds}s")
    

if __name__ == '__main__':
  parser = argparse.ArgumentParser(
    'List proper Coxeter group elements for finite families.'
  )
  parser.add_argument(
    '--type', type=str, required=True, help='Coxeter Type',
  )
  parser.add_argument(
    '-n', type=int, required=True, help='n',
  )
  parser.add_argument(
    '--in-file', type=pathlib.Path, required=True, help='File to read elements',
  )
  parser.add_argument(
    '--out-dir', type=pathlib.Path, required=True, help='File to store results',
  )
  args = parser.parse_args()
  main(args)