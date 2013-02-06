set :domain,  "jacksonriverdev.com"
set :user,    "admin"

# name this the same thing as the directory on your server
set :application, "springboard-7.x"
set :repository, "https://github.com/JacksonRiver/springboard_modules.git"

server "#{domain}", :app, :web, :db, :primary => true

set :deploy_via, :remote_cache
set :copy_exclude, [".git", ".DS_Store"]
set :scm, :git
set :branch, "7.x-3.0-qa"
set :deploy_to, "/var/www/#{application}"
set :use_sudo, false
set :keep_releases, 3
set :git_shallow_clone, 1

ssh_options[:paranoid] = false

# this tells capistrano what to do when you deploy
namespace :deploy do

  desc <<-DESC
  A macro-task that updates the code and fixes the symlink.
  DESC
  task :default do
    transaction do
      update_code
      symlink
    end
  end

  task :update_code, :except => { :no_release => true } do
    on_rollback { run "rm -rf #{release_path}; true" }
    strategy.deploy!
  end

  task :after_deploy do
    cleanup
  endß

end
