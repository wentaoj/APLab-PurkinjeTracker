script_folder="/home/wentao/Documents/purkinje-cpp/build/Release/generators"
echo "echo Restoring environment" > "$script_folder/deactivate_conanbuildenv-release-x86_64.sh"
for v in ACLOCAL_PATH AUTOMAKE_CONAN_INCLUDES AUTOCONF AUTORECONF AUTOHEADER AUTOM4TE M4
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "$script_folder/deactivate_conanbuildenv-release-x86_64.sh"
    else
        echo unset $v >> "$script_folder/deactivate_conanbuildenv-release-x86_64.sh"
    fi
done


export ACLOCAL_PATH="$ACLOCAL_PATH:/home/wentao/.conan2/p/b/libto46cac857f04b6/p/res/aclocal"
export AUTOMAKE_CONAN_INCLUDES="$AUTOMAKE_CONAN_INCLUDES:/home/wentao/.conan2/p/b/libto46cac857f04b6/p/res/aclocal"
export AUTOCONF="/home/wentao/.conan2/p/autocf2af015330354/p/bin/autoconf"
export AUTORECONF="/home/wentao/.conan2/p/autocf2af015330354/p/bin/autoreconf"
export AUTOHEADER="/home/wentao/.conan2/p/autocf2af015330354/p/bin/autoheader"
export AUTOM4TE="/home/wentao/.conan2/p/autocf2af015330354/p/bin/autom4te"
export M4="/home/wentao/.conan2/p/m43fe61932e2887/p/bin/m4"