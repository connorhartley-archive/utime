{
    'targets': [
        {
            'target_name': 'binding',
            'win_delay_load_hook': 'true',
            'sources': [
                'src/time.cpp'
            ],
            'include_dirs': [
                '<!(node -e "require(\'nan\')")'
            ]
        }
    ]
}
