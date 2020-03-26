module.exports = function (grunt) {
    require('google-closure-compiler').grunt(grunt);

    grunt.initConfig({
        'jshint': {
            files: [
                'Gruntfile.js',
                'src/assets/scripts.js'
            ],
            options: {
                globals: {
                    jQuery: true
                }
            }
        },
        'closure-compiler': {
            dist: {
                files: {
                    'dist/bundled.js': [
                        'src/assets/jquery.js',
                        'src/assets/bootstrap.js',
                        'src/assets/scripts.js'
                    ]
                },
                options: {
                    compilation_level: 'SIMPLE',
                    language_in: 'ECMASCRIPT5_STRICT',
                    warning_level: 'QUIET',
                    assume_function_wrapper: true
                }
            }
        },
        'uncss': {
            dist: {
                files: {
                    'dist/bundled.css': ['src/index.html']
                }
            }
        },
        'processhtml': {
            dist_1: {
                files: {
                    'dist/index.html': ['src/index.html']
                }
            },
            dist_2: {
                files: {
                    'dist/index.html': ['dist/index.html']
                }
            }
        },
        'copy': {
            dist: {
                files: [
                  { src: 'src/favicon.ico', dest: 'dist/favicon.ico' }
                ]
            }
        },
        'clean': ['dist/*'],
        'cssmin': {
            dist: {
                files: {
                    'dist/bundled.css': ['dist/bundled.css']
                }
            }
        },
        'htmlmin': {
            dist: {
                options: {
                    removeComments: true,
                    collapseWhitespace: true,
                    collapseBooleanAttributes: true,
                    conservativeCollapse: true,
                    minifyCSS: true
                },
                files: {
                    'dist/index.html': 'dist/index.html'
                }
            }
        },
        'compress': {
            js: {
                options: {mode: 'gzip', level: 9},
                expand: true,
                cwd: 'dist/',
                src: ['*.js'],
                dest: 'dist/',
                ext: '.js.gz'
            },
            css: {
                options: {mode: 'gzip', level: 9},
                expand: true,
                cwd: 'dist/',
                src: ['*.css'],
                dest: 'dist/',
                ext: '.css.gz'
            },
            html: {
                options: {mode: 'gzip', level: 9},
                expand: true,
                cwd: 'dist/',
                src: ['*.html'],
                dest: 'dist/',
                ext: '.html.gz'
            },
            icon: {
                options: {mode: 'gzip', level: 9},
                expand: true,
                cwd: 'dist/',
                src: ['*.ico'],
                dest: 'dist/',
                ext: '.ico.gz'
            }
        }
    });

    grunt.loadNpmTasks('grunt-contrib-jshint');
    grunt.loadNpmTasks('grunt-uncss');
    grunt.loadNpmTasks('grunt-processhtml');
    grunt.loadNpmTasks('grunt-contrib-copy');
    grunt.loadNpmTasks('grunt-contrib-clean');
    grunt.loadNpmTasks('grunt-contrib-cssmin');
    grunt.loadNpmTasks('grunt-contrib-htmlmin');
    grunt.loadNpmTasks('grunt-contrib-compress');

    grunt.registerTask('default', [
        'clean',
        'jshint',
        'closure-compiler',
        'uncss',
        'processhtml:dist_1',
        'processhtml:dist_2',
        'copy',
        'cssmin',
        'htmlmin'
    ]);
    grunt.registerTask('js', [
        'closure-compiler'
    ]);
    grunt.registerTask('html', [
        'processhtml:dist_1',
        'processhtml:dist_2',
        'htmlmin'
    ]);
    grunt.registerTask('gz', [
        'compress:js',
        'compress:css',
        'compress:html',
        //'compress:icon'
    ]);

};