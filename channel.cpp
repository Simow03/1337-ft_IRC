// #include "channel.hpp"
// class Client;
// channel::channel(std::string name) : name(name) {
// 	is_private = 0;
// 	is_invited = 0;
// 	topic = "";
// 	key = "";
// }

//  //-> constructor
// channel::channel(std::string name, std::string key, int i) : name(name), key(key) {
// 	is_private = i;
// 	topic = "";
// } //-> constructor


// void channel::Setkey(std::string k)
// {
// 	key = k;
// }
// int channel::if_invited(Client *c)
// {
//             for(size_t i = 0; i < invited.size();i++)
//             {
//                 if(invited[i] == c)
//                     return 1;
//             }
//             return 0;
// }
// void channel::add_client(Client *c)
// {
//     users.push_back(c);
// }
// int channel::client_exist(Client *c)
// {
//     for(size_t i = 0; i < users.size();i++)
//     {
//         if(users[i] == c)
//             return 1;
//     }
//     return 0;
// }

// void channel::remove_client(Client *c)
// {
//     for(size_t i = 0; i < users.size();i++)
//     {
//         if(users[i] == c)
//         {
//             users.erase(users.begin() + i);
//             return;
//         }
//     }
// }
// void channel::add_client_as_admin(Client *c)
// {
//     admins.push_back(c);
// }
// void channel::SetLimit(int limit)
// {
//     max_users = limit;
// }
// Client channel::*GetClient(std::string name)
// {
//     for(size_t i = 0; i < users.size();i++)
//     {
//         if(users[i]->getNickName() == name)
//             return users[i];
//     }
//     return NULL;
// }

// int channel::client_is_admin(Client *c)
// {
//     for(size_t i = 0; i < admins.size();i++)
//     {
//         if(admins[i] == c)
//             return 1;
//     }
//     return 0;
// }

// void channel::remove_client_as_admin(Client *c)
// {
//     for(size_t i = 0; i < admins.size();i++)
//     {
//         if(admins[i] == c)
//         {
//             admins.erase(admins.begin() + i);
//             return;
//         }
//     }
// }