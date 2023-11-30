
convert_ssmc() {
  echo "unpacking $1"
  fpath=$1
  fname=`basename ${fpath}`
  directory=`dirname ${fpath}`
  tar -xf $fpath -C $directory

  matrix_name=${fname/.tar.gz/}

  main_matrix=$directory/$matrix_name/$matrix_name.mtx

  dest_file=$directory/$matrix_name.coo.bsp.h5

  # echo "dest file ${dest_file}"

  # echo "Primary matrix is \"${main_matrix}\" -> ${dest_file}/root"
  ./convert_binsparse ${main_matrix} ${dest_file} COO

  for secondary_matrix in $directory/$matrix_name/${matrix_name}_*.mtx
  do
    # echo "Secondary matrix \"${secondary_matrix}\""
    secondary_name=`basename ${secondary_matrix}`
    secondary_name=${secondary_name/.mtx/}
    secondary_name=${secondary_name/${matrix_name}_/}
    ./convert_binsparse ${secondary_matrix} ${dest_file} COO ${secondary_name}
  done

  for text_file in $directory/$matrix_name/${matrix_name}_*.txt
  do
    #echo "Text file \"${text_file}\""
    secondary_name=`basename ${text_file}`
    secondary_name=${secondary_name/.txt/}
    secondary_name=${secondary_name/${matrix_name}_/}
    ./text2hdf5 ${text_file} ${dest_file} ${secondary_name}
  done

  rm -r ${directory}/${matrix_name}
}

convert_ssmc $1
