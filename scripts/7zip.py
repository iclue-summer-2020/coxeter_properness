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
from tqdm import tqdm

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
      with py7zr.SevenZipFile(info.archive, mode='r') as z:
        z.extract(path=temp_dirname, targets=[info.in_path])


    with Halo(text=f"processing {info.in_path}") as spinner:
      process = subprocess.run([
        RUN,
        "--type", info.coxeter_type,
        "--rank", str(info.rank),
        "--in-file", f"{temp_dirname}/{info.in_path}",
        "--out-file", info.out_path,
      ], capture_output=True)

    if process.returncode != 0:
      logger.error(f"something went wrong:\n\t{process.args=}\n\t{process.stdout=!r}\n\t{process.stderr=!r}")
      spinner.fail("something went wrong")

    spinner.text = f"removing file {temp_dirname}"
  spinner.succeed("removed file")


def main(args: argparse.Namespace) -> None:
  archive: pathlib.Path = args.archive
  out_dir: pathlib.Path = args.out_dir
  only_one: Optional[int] = args.only_one
  start_time = dt.datetime.now()

  with py7zr.SevenZipFile(archive, mode='r') as z:
    infos = z.list()

  raw_file_infos = sorted([
      ProcessInfo(
        coxeter_type=args.type,
        rank=args.n,
        in_path=info.filename,
        out_path=f"{out_dir}/{info.filename}.out",
        archive=str(archive)
      )
      for info in infos
    ],
    key=lambda info: int(info.in_path[3:])
  )

  file_infos = [
    pinfo
    for pinfo in raw_file_infos
    if only_one is not None and pinfo.in_path.endswith(str(only_one))
  ]

  with Halo(text=f"processing {args.type}{args.n}") as spinner:
    if args.max_workers == 1:
      for raw_info in tqdm(file_infos, desc="processing info"):
        info: ProcessInfo = raw_info
        process(info)
    else:
      with ThreadPoolExecutor(max_workers=args.max_workers or None) as pool:
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
    '--archive', type=pathlib.Path, required=True, help='File to read elements',
  )
  parser.add_argument(
    '--out-dir', type=pathlib.Path, required=True, help='File to store results',
  )
  parser.add_argument(
    '--max-workers', type=int, default=1, help='max parallel workers',
  )
  parser.add_argument(
    '--only-one', type=int, help='if non-None, only runs the one length specified'
  )
  args = parser.parse_args()
  main(args)