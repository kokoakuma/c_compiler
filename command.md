
 ファイル読み込みつつ、起動
 docker run --rm -it -v $HOME/github/c_compiler:/c_compiler compilerbook

 docker commit feaa7e0cff51 compilerbook

https://zenn.dev/micin/articles/78f292afb77ef0

シェルでは直前のコマンドの終了コードが$?という変数でアクセスできる

シェルのデバッグ
`bash -x ./test.sh`

bash
ここから
https://www.sigbus.info/compilerbook#%E9%96%A2%E6%95%B0%E5%91%BC%E3%81%B3%E5%87%BA%E3%81%97%E3%82%92%E5%90%AB%E3%82%80%E4%BE%8B
