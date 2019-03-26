#!/usr/bin/env bash

no_use_passwd(){
    # 禁用密码登陆
    sed -i "s/PasswordAuthentication.*/PasswordAuthentication no/g"    /etc/ssh/sshd_config
    sed -i "s/#PasswordAuthentication.*/PasswordAuthentication no/g"   /etc/ssh/sshd_config

    # 只能SSH2访问,这个安全性高.
    sed -i '/Protocol/d' /etc/ssh/sshd_config
    echo "Protocol 2" >> /etc/ssh/sshd_config

    # 重启ssh服务
    systemctl restart ssh
}

ssh_key(){
    if [ ! -e '/root/.ssh/authorized_keys' ]; then
    mkdir -p /root/.ssh
    cd /root/.ssh
    cat <<EOF > authorized_keys
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDm5IXhVxCp317F3X+VEKmLPNiIGigbX4eot3EPIizbHHVURVOEXbxlN5tqQPmFt62qex4GhXmOfcMmLcaDwrkci7A3dPlnjRukYBDK3jQtzjVexTvNqcblTKj2aEpZg5Z8Z5vCOVrOfCdXOngqi/NkItvsid2DLzqi7KTpdFpgZGSwpV2h3E3U0N18uBLQwDj3ag2mUT7X2kx4t4DYbQDWyGt+bbivnTHV0k/BZJPwqnPD2tpyH19lKWvL3fm3gjoMkTtxDzEXHpTmTe95Z1AKzOfY1reEZ+dHAWy0wHmCA1by1XJtagoeTVqa+IS1wfOimwJE/oIofhDjSNjsMk63 root
EOF
    fi
}

ssh_key
no_use_passwd
