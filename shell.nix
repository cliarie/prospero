{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = [ pkgs.eigen ];
  shellHook = ''
    export EIGEN3_INCLUDE_DIR=$(find ${pkgs.eigen}/include -type d -name Eigen | head -n1 | xargs dirname)
    echo "Eigen include dir set to $EIGEN3_INCLUDE_DIR"
  '';
}
